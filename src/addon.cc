#include <node.h>
#include "vtchanges.h"

using namespace v8;

Handle<Value> CreateObject(const Arguments& args) {
  HandleScope scope;
  return scope.Close(VTChanges::NewInstance(args));
}

void InitAll(Handle<Object> exports, Handle<Object> module) {
  VTChanges::Init();

  module->Set(String::NewSymbol("exports"),
      FunctionTemplate::New(CreateObject)->GetFunction());
}

NODE_MODULE(vtchanges, InitAll)
