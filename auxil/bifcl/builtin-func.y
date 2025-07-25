%{
#include <vector>
#include <set>
#include <string>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>

#include "module_util.h"

using namespace std;

extern int line_number;
extern char* input_filename;
extern char* input_filename_with_path;
extern char* plugin;
extern bool alternative_mode;

#define print_line_directive(fp) fprintf(fp, "\n#line %d \"%s\"\n", line_number, input_filename_with_path)

extern FILE* fp_zeek_init;
extern FILE* fp_func_def;
extern FILE* fp_func_h;
extern FILE* fp_func_init;
extern FILE* fp_netvar_h;
extern FILE* fp_netvar_def;
extern FILE* fp_netvar_init;

bool in_c_code = false;
string current_module = GLOBAL_MODULE_NAME;
int definition_type;
string type_name;

// Alternate event prototypes are only written to the .zeek file, but
// don't need any further changes to C++ source/header files, so this
// set keeps track of whether the first event prototype information has
// already been defined/written to the C++ files.
static std::set<std::string> events;

enum {
	C_SEGMENT_DEF,
	FUNC_DEF,
	EVENT_DEF,
	TYPE_DEF,
	CONST_DEF,
};

// Holds the name of a declared object (function, enum, record type, event,
// etc. and information about namespaces, etc.
struct decl_struct {
	string module_name;
	string bare_name; // name without module or namespace
	string c_namespace_start; // "opening" namespace for use in netvar_*
	string c_namespace_end;   // closing "}" for all the above namespaces
	string c_fullname; // fully qualified name (namespace::....) for use in netvar_init
	string zeek_fullname; // fully qualified zeek name, for netvar (and lookup_ID())
	string zeek_name;  // the name as we read it from input. What we write into the .zeek file

	// special cases for events. Events have an EventHandlerPtr
	// and a enqueue_* function. This name is for the enqueue_* function
	string enqueue_c_namespace_start;
	string enqueue_c_namespace_end;
	string enqueue_c_barename;
	string enqueue_c_fullname;
} decl;

void set_definition_type(int type, const char *arg_type_name)
	{
	definition_type = type;
	if ( type == TYPE_DEF && arg_type_name )
		type_name = string(arg_type_name);
	else
		type_name = "";
	}

void set_decl_name(const char *name)
	{
	decl.bare_name = extract_var_name(name);

	// make_full_var_name prepends the correct module, if any
	// then we can extract the module name again.
	string varname = make_full_var_name(current_module.c_str(), name);
	decl.module_name = extract_module_name(varname.c_str());

	decl.c_namespace_start = "";
	decl.c_namespace_end = "";
	decl.c_fullname = "";
	decl.zeek_fullname = "";
	decl.zeek_name = "";

	decl.enqueue_c_fullname = "";
	decl.enqueue_c_barename = string("enqueue_") + decl.bare_name;
	decl.enqueue_c_namespace_start = "";
	decl.enqueue_c_namespace_end = "";

	switch ( definition_type ) {
	case TYPE_DEF:
		decl.c_namespace_start = "namespace BifType { namespace " + type_name + "{ ";
		decl.c_namespace_end = " } }";
		decl.c_fullname = "BifType::" + type_name + "::";
		break;

	case CONST_DEF:
		decl.c_namespace_start = "namespace BifConst { ";
		decl.c_namespace_end = " } ";
		decl.c_fullname = "BifConst::";
		break;

	case FUNC_DEF:
		decl.c_namespace_start = "namespace BifFunc { ";
		decl.c_namespace_end = " } ";
		decl.c_fullname = "BifFunc::";
		break;

	case EVENT_DEF:
		decl.c_namespace_start = "";
		decl.c_namespace_end = "";
		decl.c_fullname = "::";  // need this for namespace qualified events due do event_c_body
		decl.enqueue_c_namespace_start = "namespace BifEvent { ";
		decl.enqueue_c_namespace_end = " } ";
		decl.enqueue_c_fullname = "zeek::BifEvent::";
		break;

	default:
		break;
	}

	if ( decl.module_name != GLOBAL_MODULE_NAME )
		{
		decl.c_namespace_start += "namespace " + decl.module_name + " { ";
		decl.c_namespace_end += string(" }");
		decl.c_fullname += decl.module_name + "::";
		decl.zeek_fullname += decl.module_name + "::";

		decl.enqueue_c_namespace_start  += "namespace " + decl.module_name + " { ";
		decl.enqueue_c_namespace_end += " } ";
		decl.enqueue_c_fullname += decl.module_name + "::";
		}

	decl.zeek_fullname += decl.bare_name;
	decl.c_fullname += decl.bare_name;
	decl.zeek_name += name;
	decl.enqueue_c_fullname += decl.enqueue_c_barename;
	}

const char* arg_list_name = "BiF_ARGS";

#include "bif_arg.h"

/* Map bif/zeek type names to C types for use in const declaration */
static struct {
	const char* bif_type;
	const char* zeek_type;
	const char* c_type;
	const char* c_type_smart;
	const char* accessor;
	const char* accessor_smart;
	const char* cast_smart;
	const char* constructor;
	const char* ctor_smatr;
} builtin_types[] = {
#define DEFINE_BIF_TYPE(id, bif_type, zeek_type, c_type, c_type_smart, accessor, accessor_smart, cast_smart, constructor, ctor_smart) \
	{bif_type, zeek_type, c_type, c_type_smart, accessor, accessor_smart, cast_smart, constructor, ctor_smart},
#include "bif_type.def"
#undef DEFINE_BIF_TYPE
};

int get_type_index(const char *type_name)
	{
	for ( int i = 0; builtin_types[i].bif_type[0] != '\0'; ++i )
		{
		if ( strcmp(builtin_types[i].bif_type, type_name) == 0 )
			return i;
		}
		return TYPE_OTHER;
	}


int var_arg; // whether the number of arguments is variable
std::vector<BuiltinFuncArg*> args;

extern int yyerror(const char[]);
extern int yywarn(const char msg[]);
extern int yylex();

char* concat(const char* str1, const char* str2)
	{
	int len1 = strlen(str1);
	int len2 = strlen(str2);

	char* s = new char[len1 + len2 +1];

	memcpy(s, str1, len1);
	memcpy(s + len1, str2, len2);

	s[len1+len2] = '\0';

	return s;
	}

static void print_event_c_prototype_args(FILE* fp)
	{
	for ( auto i = 0u; i < args.size(); ++i )
		{
		if ( i > 0 )
			fprintf(fp, ", ");

		args[i]->PrintCArg(fp, i);
		}
	}

static void print_event_c_prototype_header(FILE* fp)
	{
	fprintf(fp, "namespace zeek { %s void %s(zeek::analyzer::Analyzer* analyzer%s",
	        decl.enqueue_c_namespace_start.c_str(),
	        decl.enqueue_c_barename.c_str(),
	        args.size() ? ", " : "" );

	print_event_c_prototype_args(fp);
	fprintf(fp, ")");
	fprintf(fp, "; %s }\n", decl.enqueue_c_namespace_end.c_str());
	}

static void print_event_c_prototype_impl(FILE* fp)
	{
	fprintf(fp, "void %s(zeek::analyzer::Analyzer* analyzer%s",
	        decl.enqueue_c_fullname.c_str(),
	        args.size() ? ", " : "" );

	print_event_c_prototype_args(fp);
	fprintf(fp, ")");
	fprintf(fp, "\n");
	}

static void print_event_c_body(FILE* fp)
	{
	fprintf(fp, "\t{\n");
	fprintf(fp, "\t// Note that it is intentional that here we do not\n");
	fprintf(fp, "\t// check if %s is NULL, which should happen *before*\n",
		decl.c_fullname.c_str());
	fprintf(fp, "\t// %s is called to avoid unnecessary Val\n",
		decl.enqueue_c_fullname.c_str());
	fprintf(fp, "\t// allocation.\n");
	fprintf(fp, "\n");

	BuiltinFuncArg* connection_arg = nullptr;

	fprintf(fp, "\tzeek::event_mgr.Enqueue(%s, zeek::Args{\n", decl.c_fullname.c_str());

	for ( int i = 0; i < (int) args.size(); ++i )
		{
		fprintf(fp, "\t        ");
		args[i]->PrintValConstructor(fp);
		fprintf(fp, ",\n");

		if ( args[i]->Type() == TYPE_CONNECTION )
			{
			if ( connection_arg == nullptr )
				connection_arg = args[i];
			else
				{
				// We are seeing two connection type arguments.
				yywarn("Warning: with more than connection-type "
				       "event arguments, bifcl only passes "
				       "the first one to EventMgr as cookie.");
				}
			}
		}

	fprintf(fp, "\t        },\n\t    zeek::util::detail::SOURCE_LOCAL, analyzer ? analyzer->GetID() : 0");

	if ( connection_arg )
		// Pass the connection to the EventMgr as the "cookie"
		fprintf(fp, ", %s", connection_arg->Name());

	fprintf(fp, ");\n");
	fprintf(fp, "\t}\n\n");
	//fprintf(fp, "%s // end namespace\n", decl.enqueue_c_namespace_end.c_str());
	}

void record_bif_item(const char* id, const char* type)
	{
	if ( ! plugin )
		return;

	fprintf(fp_func_init, "\tplugin->AddBifItem(\"%s\", zeek::plugin::BifItem::%s);\n", id, type);
	}

%}

%token TOK_LPP TOK_RPP TOK_LPB TOK_RPB TOK_LPPB TOK_RPPB TOK_VAR_ARG
%token TOK_BOOL
%token TOK_FUNCTION TOK_EVENT TOK_CONST TOK_ENUM TOK_OF
%token TOK_TYPE TOK_RECORD TOK_SET TOK_VECTOR TOK_OPAQUE TOK_TABLE TOK_MODULE
%token TOK_ARGS TOK_ARG TOK_ARGC
%token TOK_ID TOK_ATTR TOK_CSTR TOK_LF TOK_WS TOK_COMMENT
%token TOK_ATOM TOK_INT TOK_C_TOKEN

%left ',' ':'

%type <str> TOK_C_TOKEN TOK_ID TOK_CSTR TOK_WS TOK_COMMENT TOK_ATTR TOK_INT opt_ws type attr_list opt_attr_list opt_func_attrs
%type <val> TOK_ATOM TOK_BOOL

%union	{
	const char* str;
	int val;
}

%%

builtin_lang:	definitions
			{
			fprintf(fp_zeek_init, "} # end of export section\n");
			fprintf(fp_zeek_init, "module %s;\n", GLOBAL_MODULE_NAME);
			}



definitions:	definitions definition opt_ws
			{
			if ( in_c_code )
				fprintf(fp_func_def, "%s", $3);
			else
				fprintf(fp_zeek_init, "%s", $3);
			}
	|	opt_ws
			{
			fprintf(fp_zeek_init, "export {\n");
			fprintf(fp_zeek_init, "%s", $1);
			}
	;

definition:	event_def
	|	func_def
	|	c_code_segment
	|	enum_def
	|	const_def
	|	type_def
	|   module_def
	;


module_def:	TOK_MODULE opt_ws TOK_ID opt_ws ';'
			{
			current_module = string($3);
			fprintf(fp_zeek_init, "module %s;\n", $3);
			}

	 // XXX: Add the netvar glue so that the event engine knows about
	 // the type. One still has to define the type in zeek.init.
	 // Would be nice, if we could just define the record type here
	 // and then copy to the .bif.zeek file, but type declarations in
	 // Zeek can be quite powerful. Don't know whether it's worth it
	 // extend the bif-language to be able to handle that all....
	 // Or we just support a simple form of record type definitions
	 // TODO: add other types (tables, sets)
type_def:	TOK_TYPE opt_ws TOK_ID opt_ws ':' opt_ws type_def_types opt_ws ';'
			{
			set_decl_name($3);

			fprintf(fp_netvar_h, "namespace zeek { %s extern zeek::IntrusivePtr<zeek::%sType> %s; %s}\n",
				decl.c_namespace_start.c_str(), type_name.c_str(),
				decl.bare_name.c_str(), decl.c_namespace_end.c_str());

			fprintf(fp_netvar_def, "namespace zeek { %s zeek::IntrusivePtr<zeek::%sType> %s; %s}\n",
				decl.c_namespace_start.c_str(), type_name.c_str(),
				decl.bare_name.c_str(), decl.c_namespace_end.c_str());
			fprintf(fp_netvar_def, "%s zeek::%sType * %s; %s\n",
				decl.c_namespace_start.c_str(), type_name.c_str(),
				decl.bare_name.c_str(), decl.c_namespace_end.c_str());

			fprintf(fp_netvar_init,
				"\tzeek::%s = zeek::id::find_type<zeek::%sType>(\"%s\");\n",
				decl.c_fullname.c_str(), type_name.c_str(),
				decl.zeek_fullname.c_str());

			record_bif_item(decl.zeek_fullname.c_str(), "TYPE");
			}
	;

type_def_types: TOK_RECORD
			{ set_definition_type(TYPE_DEF, "Record"); }
	| TOK_SET
			{ set_definition_type(TYPE_DEF, "Set"); }
	| TOK_VECTOR
			{ set_definition_type(TYPE_DEF, "Vector"); }
	| TOK_TABLE
			{ set_definition_type(TYPE_DEF, "Table"); }
	;

opt_func_attrs:	attr_list opt_ws
		{ $$ = $1; }
	| /* nothing */
		{ $$ = ""; }
	;

event_def:	event_prefix opt_ws plain_head opt_func_attrs
			{ fprintf(fp_zeek_init, "%s", $4); } end_of_head ';'
			{
			if ( events.find(decl.zeek_fullname) == events.end() )
				{
				print_event_c_prototype_header(fp_func_h);
				print_event_c_prototype_impl(fp_func_def);
				print_event_c_body(fp_func_def);
				events.insert(decl.zeek_fullname);
				}
			}

func_def:	func_prefix opt_ws typed_head opt_func_attrs
			{ fprintf(fp_zeek_init, "%s", $4); } end_of_head body
	;

enum_def:	enum_def_1 enum_list TOK_RPB opt_attr_list
			{
			// First, put an end to the enum type decl.
			fprintf(fp_zeek_init, "} ");
			fprintf(fp_zeek_init, "%s", $4);
			fprintf(fp_zeek_init, ";\n");
			if ( decl.module_name != GLOBAL_MODULE_NAME )
				fprintf(fp_netvar_h, "}; } }\n");
			else
				fprintf(fp_netvar_h, "}; }\n");

			// Now generate the netvar's.
			fprintf(fp_netvar_h, "namespace zeek { %s extern zeek::IntrusivePtr<zeek::EnumType> %s; %s}\n",
				decl.c_namespace_start.c_str(), decl.bare_name.c_str(), decl.c_namespace_end.c_str());
			fprintf(fp_netvar_def, "namespace zeek { %s zeek::IntrusivePtr<zeek::EnumType> %s; %s}\n",
				decl.c_namespace_start.c_str(), decl.bare_name.c_str(), decl.c_namespace_end.c_str());
			fprintf(fp_netvar_def, "%s zeek::EnumType * %s; %s\n",
				decl.c_namespace_start.c_str(), decl.bare_name.c_str(), decl.c_namespace_end.c_str());

			fprintf(fp_netvar_init,
				"\tzeek::%s = zeek::id::find_type<zeek::EnumType>(\"%s\");\n",
				decl.c_fullname.c_str(), decl.zeek_fullname.c_str());

			record_bif_item(decl.zeek_fullname.c_str(), "TYPE");
			}
	;

enum_def_1:	TOK_ENUM opt_ws TOK_ID opt_ws TOK_LPB opt_ws
			{
			set_definition_type(TYPE_DEF, "Enum");
			set_decl_name($3);
			fprintf(fp_zeek_init, "type %s: enum %s{%s", decl.zeek_name.c_str(), $4, $6);

			// this is the namespace were the enumerators are defined, not where
			// the type is defined.
			// We don't support fully qualified names as enumerators. Use a module name
			fprintf(fp_netvar_h, "namespace BifEnum { ");
			if ( decl.module_name != GLOBAL_MODULE_NAME )
				fprintf(fp_netvar_h, "namespace %s { ", decl.module_name.c_str());
			fprintf(fp_netvar_h, "enum %s {\n", $3);
			}
	;

enum_list:	enum_list TOK_ID opt_ws ',' opt_ws
			{
			fprintf(fp_zeek_init, "%s%s,%s", $2, $3, $5);
			fprintf(fp_netvar_h, "\t%s,\n", $2);
			}
	| 		enum_list TOK_ID opt_ws '=' opt_ws TOK_INT opt_ws ',' opt_ws
			{
			fprintf(fp_zeek_init, "%s = %s%s,%s", $2, $6, $7, $9);
			fprintf(fp_netvar_h, "\t%s = %s,\n", $2, $6);
			}
	|	/* nothing */
	;


const_def:	TOK_CONST opt_ws TOK_ID opt_ws ':' opt_ws TOK_ID opt_ws ';'
			{
			set_definition_type(CONST_DEF, 0);
			set_decl_name($3);
			int typeidx = get_type_index($7);
			char accessor[1024];
			char accessor_smart[1024];

			snprintf(accessor, sizeof(accessor), builtin_types[typeidx].accessor, "");
			snprintf(accessor_smart, sizeof(accessor_smart), builtin_types[typeidx].accessor_smart, "");


			fprintf(fp_netvar_h, "namespace zeek { %s extern %s %s; %s }\n",
					decl.c_namespace_start.c_str(),
					builtin_types[typeidx].c_type_smart, decl.bare_name.c_str(),
					decl.c_namespace_end.c_str());

			fprintf(fp_netvar_def, "namespace zeek { %s %s %s; %s }\n",
					decl.c_namespace_start.c_str(),
					builtin_types[typeidx].c_type_smart, decl.bare_name.c_str(),
					decl.c_namespace_end.c_str());
			fprintf(fp_netvar_def, "%s %s %s; %s\n",
					decl.c_namespace_start.c_str(),
					builtin_types[typeidx].c_type, decl.bare_name.c_str(),
					decl.c_namespace_end.c_str());

			if ( alternative_mode && ! plugin )
				fprintf(fp_netvar_init, "\tzeek::detail::bif_initializers.emplace_back([]()\n");

			fprintf(fp_netvar_init, "\t{\n");
			fprintf(fp_netvar_init, "\tconst auto& v = zeek::id::find_const%s(\"%s\");\n",
					builtin_types[typeidx].cast_smart, decl.zeek_fullname.c_str());
			fprintf(fp_netvar_init, "\tzeek::%s = v%s;\n",
					decl.c_fullname.c_str(), accessor_smart);
			fprintf(fp_netvar_init, "\t}\n");

			if ( alternative_mode && ! plugin )
				fprintf(fp_netvar_init, "\t);\n");

			record_bif_item(decl.zeek_fullname.c_str(), "CONSTANT");
			}

attr_list:
		attr_list TOK_ATTR
			{ $$ = concat($1, $2); }
	|
		TOK_ATTR
	;

opt_attr_list:
		attr_list
	|	/* nothing */
		{ $$ = ""; }
	;

func_prefix:	TOK_FUNCTION
			{ set_definition_type(FUNC_DEF, 0); }
	;

event_prefix:	TOK_EVENT
			{ set_definition_type(EVENT_DEF, 0); }
	;

end_of_head:	/* nothing */
			{
			fprintf(fp_zeek_init, ";\n");
			}
	;

typed_head:	plain_head return_type
			{
			}
	;

plain_head:	head_1 args arg_end opt_ws
			{
			if ( var_arg )
				fprintf(fp_zeek_init, "va_args: any");
			else
				{
				for ( int i = 0; i < (int) args.size(); ++i )
					{
					if ( i > 0 )
						fprintf(fp_zeek_init, ", ");
					args[i]->PrintZeek(fp_zeek_init);
					}
				}

			fprintf(fp_zeek_init, ")");

			fprintf(fp_zeek_init, "%s", $4);
			fprintf(fp_func_def, "%s", $4);
			}
	;

head_1:		TOK_ID opt_ws arg_begin
			{
			const char* method_type = nullptr;
			set_decl_name($1);

			if ( definition_type == FUNC_DEF )
				{
				method_type = "function";
				print_line_directive(fp_func_def);
				}
			else if ( definition_type == EVENT_DEF )
				method_type = "event";

			if ( method_type )
				fprintf(fp_zeek_init,
					"global %s: %s%s(",
					decl.zeek_name.c_str(), method_type, $2);

			if ( definition_type == FUNC_DEF )
				{
				fprintf(fp_func_init,
					"\t(void) new zeek::detail::BuiltinFunc(zeek::%s_bif, \"%s\", 0);\n",
					decl.c_fullname.c_str(), decl.zeek_fullname.c_str());

				// This is the "canonical" version, with argument type and order
				// mostly for historical reasons.  There's also no "zeek_" prefix
				// in the function name itself, but does have a "_bif" suffix
				// to potentially help differentiate from other functions
				// (e.g. ones at global scope that may be used to implement
				// the BIF itself).
				fprintf(fp_func_h,
					"namespace zeek { %sextern zeek::detail::BifReturnVal %s_bif(zeek::detail::Frame* frame, const zeek::Args*);%s }\n",
					decl.c_namespace_start.c_str(), decl.bare_name.c_str(), decl.c_namespace_end.c_str());

				fprintf(fp_func_def,
					"zeek::detail::BifReturnVal zeek::%s_bif(zeek::detail::Frame* frame, const zeek::Args* %s)",
					decl.c_fullname.c_str(), arg_list_name);

				record_bif_item(decl.zeek_fullname.c_str(), "FUNCTION");
				}
			else if ( definition_type == EVENT_DEF )
				{
				if ( events.find(decl.zeek_fullname) == events.end() )
					{
					// TODO: add namespace for events here
					fprintf(fp_netvar_h,
						"%sextern zeek::EventHandlerPtr %s; %s\n",
						decl.c_namespace_start.c_str(), decl.bare_name.c_str(), decl.c_namespace_end.c_str());

					fprintf(fp_netvar_def,
					        "%szeek::EventHandlerPtr %s; %s\n",
					        decl.c_namespace_start.c_str(), decl.bare_name.c_str(), decl.c_namespace_end.c_str());

					fprintf(fp_netvar_init,
						"\t%s = zeek::event_registry->Register(\"%s\");\n",
						decl.c_fullname.c_str(), decl.zeek_fullname.c_str());

					record_bif_item(decl.zeek_fullname.c_str(), "EVENT");
					// C++ prototypes of zeek_event_* functions will
					// be generated later.
					}
				}
			}
	;

arg_begin:	TOK_LPP
			{ args.clear(); var_arg = 0; }
	;

arg_end:	TOK_RPP
	;

args:		args_1
	|	opt_ws
			{ /* empty, to avoid yacc complaint about type clash */ }
	;

args_1:		args_1 ',' opt_ws arg opt_ws opt_attr_list
			{ if ( ! args.empty() ) args[args.size()-1]->SetAttrStr($6); }
	|	opt_ws arg opt_ws opt_attr_list
			{ if ( ! args.empty() ) args[args.size()-1]->SetAttrStr($4); }
	;

// TODO: Migrate all other compound types to this rule. Once the BiF language
// can parse all regular Zeek types, we can throw out the unnecessary
// boilerplate typedefs for addr_set, string_set, etc.
type:
                TOK_OPAQUE opt_ws TOK_OF opt_ws TOK_ID
                        { $$ = concat("opaque of ", $5); }
        |       TOK_ID
                        { $$ = $1; }
        ;

arg:		TOK_ID opt_ws ':' opt_ws type
			{ args.push_back(new BuiltinFuncArg($1, $5)); }
	|	TOK_VAR_ARG
			{
			if ( definition_type == EVENT_DEF )
				yyerror("events cannot have variable arguments");
			var_arg = 1;
			}
	;

return_type:	':' opt_ws type opt_ws
			{
			BuiltinFuncArg* ret = new BuiltinFuncArg("", $3);
			ret->PrintZeek(fp_zeek_init);
			delete ret;
			fprintf(fp_func_def, "%s", $4);
			}
	;

body:		body_start c_body body_end
			{
			fprintf(fp_func_def, " // end of %s\n", decl.c_fullname.c_str());
			print_line_directive(fp_func_def);
			}
	;

c_code_begin:	/* empty */
			{
			in_c_code = true;
			print_line_directive(fp_func_def);
			}
	;

c_code_end:	/* empty */
			{ in_c_code = false; }
	;

body_start:	TOK_LPB c_code_begin
			{
			int implicit_arg = 0;
			int argc = args.size();

			fprintf(fp_func_def, "{");

			if ( argc > 0 || ! var_arg )
				fprintf(fp_func_def, "\n");

			if ( ! var_arg )
				{
				fprintf(fp_func_def, "\tif ( %s->size() != %d )\n", arg_list_name, argc);
				fprintf(fp_func_def, "\t\t{\n");
				fprintf(fp_func_def,
					"\t\tzeek::emit_builtin_error(zeek::util::fmt(\"%s() takes exactly %d argument(s), got %%lu\", %s->size()));\n",
					decl.zeek_fullname.c_str(), argc, arg_list_name);
				fprintf(fp_func_def, "\t\treturn nullptr;\n");
				fprintf(fp_func_def, "\t\t}\n");
				}
			else if ( argc > 0 )
				{
				fprintf(fp_func_def, "\tif ( %s->size() < %d )\n", arg_list_name, argc);
				fprintf(fp_func_def, "\t\t{\n");
				fprintf(fp_func_def,
					"\t\tzeek::emit_builtin_error(zeek::util::fmt(\"%s() takes at least %d argument(s), got %%lu\", %s->size()));\n",
					decl.zeek_fullname.c_str(), argc, arg_list_name);
				fprintf(fp_func_def, "\t\treturn nullptr;\n");
				fprintf(fp_func_def, "\t\t}\n");
				}

			for ( int i = 0; i < (int) args.size(); ++i )
				args[i]->PrintCDef(fp_func_def, i + implicit_arg, var_arg);
			print_line_directive(fp_func_def);
			}
	;

body_end:	TOK_RPB c_code_end
			{
			fprintf(fp_func_def, "}");
			}
	;

c_code_segment: TOK_LPPB c_code_begin c_body c_code_end TOK_RPPB
	;

c_body:		opt_ws
			{ fprintf(fp_func_def, "%s", $1); }
	|	c_body c_atom opt_ws
			{ fprintf(fp_func_def, "%s", $3); }
	;

c_atom:		TOK_ID
			{ fprintf(fp_func_def, "%s", $1); }
	|	TOK_C_TOKEN
			{ fprintf(fp_func_def, "%s", $1); }
	|	TOK_ARG
			{ fprintf(fp_func_def, "(*%s)", arg_list_name); }
	|	TOK_ARGS
			{ fprintf(fp_func_def, "%s", arg_list_name); }
	|	TOK_ARGC
			{ fprintf(fp_func_def, "%s->size()", arg_list_name); }
	|	TOK_CSTR
			{ fprintf(fp_func_def, "%s", $1); }
	|	TOK_ATOM
			{ fprintf(fp_func_def, "%c", $1); }
	|	TOK_INT
			{ fprintf(fp_func_def, "%s", $1); }

	;

opt_ws:		opt_ws TOK_WS
			{ $$ = concat($1, $2); }
	|	opt_ws TOK_LF
			{ $$ = concat($1, "\n"); }
	|	opt_ws TOK_COMMENT
			{
			if ( in_c_code )
				$$ = concat($1, $2);
			else
				if ( $2[1] == '#' )
					// This is a special type of comment that is used to
					// generate zeek script documentation, so pass it through.
					$$ = concat($1, $2);
				else
					$$ = $1;
			}
	|	/* empty */
			{ $$ = ""; }
	;

%%

extern char* yytext;
extern char* input_filename;
extern int line_number;
void err_exit(void);

void print_msg(const char msg[])
	{
	int msg_len = strlen(msg) + strlen(yytext) + 64;
	char* msgbuf = new char[msg_len];

	if ( yytext[0] == '\n' )
		snprintf(msgbuf, msg_len, "%s, on previous line", msg);

	else if ( yytext[0] == '\0' )
		snprintf(msgbuf, msg_len, "%s, at end of file", msg);

	else
		snprintf(msgbuf, msg_len, "%s, at or near \"%s\"", msg, yytext);

	/*
	extern int column;
	sprintf(msgbuf, "%*s\n%*s\n", column, "^", column, msg);
	*/

	if ( input_filename )
		fprintf(stderr, "%s:%d: ", input_filename, line_number);
	else
		fprintf(stderr, "line %d: ", line_number);
	fprintf(stderr, "%s\n", msgbuf);

	delete [] msgbuf;
	}

int yywarn(const char msg[])
	{
	print_msg(msg);
	return 0;
	}

int yyerror(const char msg[])
	{
	print_msg(msg);

	err_exit();
	return 0;
	}
