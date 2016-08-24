

#include <clang-c/Index.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

using namespace std;

unsigned editingOptions() {
  return CXTranslationUnit_DetailedPreprocessingRecord |
	 CXTranslationUnit_Incomplete |
	 CXTranslationUnit_IncludeBriefCommentsInCodeCompletion |
	 clang_defaultEditingTranslationUnitOptions();
}

typedef std::vector<std::pair<std::string,std::string>> plugin_list;

void add_load_flags_for_lib( plugin_list& list, std::vector<const char*>& pointer_flags )
{
  for( auto& element : list ){
#if 0
    pointer_flags.push_back("-Xclang");
    pointer_flags.push_back("-load");
    pointer_flags.push_back("-Xclang");
    pointer_flags.push_back(element.first.c_str());
#else
    //pointer_flags.push_back(strdup(("-fplugin="s + element.first).c_str()));
#endif
    pointer_flags.push_back("-Xclang");
    pointer_flags.push_back("-add-plugin");
    pointer_flags.push_back("-Xclang");
    pointer_flags.push_back(element.second.c_str());
  }
  
}

int main(int argc, char** argv) {
  CXIndex clang_index = clang_createIndex(0,0);
  std::string filename = "test.cpp";
  std::vector<const char*> pointer_flags;

  plugin_list lib_and_pluginname_list;
  //lib_and_pluginname_list.emplace_back( make_pair( "/home/incubus/dev/ClanPlugin/lib/ClanPlugin.so"s, "clan"s ));
#if 1
  lib_and_pluginname_list.emplace_back( make_pair( "/home/incubus/llvm_patch_test/build/lib/clangTidyPluginLM.so"s, "clang-tidy"s ));
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-plugin-arg-clang-tidy");
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-checks=*");
#endif

  add_load_flags_for_lib( lib_and_pluginname_list, pointer_flags );

#if 0
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-plugin-arg-clan");
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-redirect-stdout");
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-plugin-arg-clan");
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("/dev/null");

  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-plugin-arg-clan");
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-redirect-stderr");
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-plugin-arg-clan");
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("/dev/null");
#endif

  std::cout << "starting to parse the tu" << std::endl;
  
  CXTranslationUnit tu = clang_createTranslationUnit(clang_index, filename.c_str() ) ;

  auto clang_translation_unit_error = clang_parseTranslationUnit2(
      clang_index, 
      filename.c_str(), 
      &pointer_flags[0], 
      pointer_flags.size(),
      nullptr, 
      0,
      clang_defaultEditingTranslationUnitOptions(),
      &tu
  );

  if ( clang_translation_unit_error != CXError_Success  ) {
    std::cout << "could not parse the tu ec: " << clang_translation_unit_error << std::endl;
    exit(-1);
  }

  unsigned int n_diagnostics = clang_getNumDiagnostics( tu );

  std::cout << "number of diagnostics " << n_diagnostics << std::endl;

  ofstream out("diag_results.txt");
  
  for (int i = 0; i < n_diagnostics; ++i){
    auto diag = clang_getDiagnostic( tu, i );
    auto diag_cxstring = clang_formatDiagnostic( diag, CXDiagnostic_DisplaySourceLocation );
    auto str = clang_getCString( diag_cxstring );
    out << str << std::endl;
  }

  out.close();

  std::cout << "done parsing the tu" << std::endl;

  return 0;
}
