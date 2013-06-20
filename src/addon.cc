#include <node.h>
#include "vtchanges.h"

using namespace v8;

Handle<Value> CreateObject(const Arguments& args) {
  HandleScope scope;
  return scope.Close(VTChanges::NewInstance(args));
}

void InitAll(Handle<Object> exports) {
  VTChanges::Init();

  exports->Set(String::NewSymbol("create"),
    FunctionTemplate::New(CreateObject)->GetFunction());
}

NODE_MODULE(vtchanges, InitAll)
