

#include <clang-c/Index.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

using namespace std;

#if 1
typedef std::vector<std::string> plugin_list;

inline void add_load_flags_for_lib( plugin_list& list, std::vector<const char*>& pointer_flags )
{
  for( auto& element : list ){
    std::cout << " exec" << std::endl;
    pointer_flags.push_back(strdup((std::string("-fplugin=") + element).c_str()));
    //pointer_flags.push_back(strdup(("-Xclang -load -Xclang "s + element).c_str()));
    //pointer_flags.push_back(element.c_str());
  }
  
}
#endif

int main(int argc, char** argv) {
  CXIndex clang_index = clang_createIndex(0,0);
  std::string filename = "main.cpp";
  std::vector<const char*> pointer_flags;

  plugin_list lib_list;
  lib_list.push_back( "/home/incubus/dev/ClanPlugin/lib/ClanPlugin.so" );
  //lib_list.emplace_back( "/home/incubus/llvm_patch_test/build/lib/clangTidyPluginLM.so" ));
  
  //add_load_flags_for_lib( lib_list, pointer_flags );

#if 0
  for( auto& element : pointer_flags ){
      std::cout << element << std::endl;
  }
#endif

  pointer_flags.push_back("-std=c++11");

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
  
  CXTranslationUnit tu = clang_createTranslationUnit(clang_index, strdup(filename.c_str()) ) ;

  auto clang_translation_unit_error = clang_parseTranslationUnit2(
      clang_index, 
      strdup(filename.c_str()), 
      nullptr,//&pointer_flags[0], 
      0,//pointer_flags.size(),
      nullptr, 
      0,
      clang_defaultEditingTranslationUnitOptions(),
      &tu
  );

  std::cout << "done parsing" << std::endl;

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


  return 0;
}
