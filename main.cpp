
#include <clang-c/Index.h>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {
  CXIndex clang_index = clang_createIndex(0,0);
  std::string filename = "test.cpp";
  std::vector<const char*> pointer_flags;

  //pointer_flags.push_back("-Xclang");
  //pointer_flags.push_back("-load");

  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-add-plugin");
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("clang-tidy");

  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-plugin-arg-clang-tidy");
  pointer_flags.push_back("-Xclang");
  pointer_flags.push_back("-checks=*");
  
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

  for (int i = 0; i < n_diagnostics; ++i){
    auto diag = clang_getDiagnostic( tu, i );
    auto diag_cxstring = clang_formatDiagnostic( diag, CXDiagnostic_DisplaySourceLocation );
    auto str = clang_getCString( diag_cxstring );
    std::cout << str << std::endl;
  }

  return 0;
}
