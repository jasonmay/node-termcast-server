#include <stdlib.h>
#include <stdio.h>
#include <node.h>

#include "vtchanges.h"

using namespace v8;

VTChanges::VTChanges() {};
VTChanges::~VTChanges() {};

Persistent<Function> VTChanges::constructor;

void VTChanges::Init() {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("VTChanges"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("process"),
      FunctionTemplate::New(Process)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("finish"),
      FunctionTemplate::New(Finish)->GetFunction());

  constructor = Persistent<Function>::New(tpl->GetFunction());
}

Handle<Value> VTChanges::New(const Arguments& args) {
  HandleScope scope;

  VTChanges* obj = new VTChanges();
#ifdef VTCHANGES_DEBUG
  fprintf(stderr, "Allocating a VT");
#endif
  obj->vt = vterm_new(80, 24);
  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> VTChanges::NewInstance(const Arguments& args) {
  HandleScope scope;

  const unsigned argc = 1;
  Handle<Value> argv[argc] = { args[0] };
  Local<Object> instance = constructor->NewInstance(argc, argv);

  return scope.Close(instance);
}

Handle<Value> VTChanges::Process(const Arguments& args) {
  HandleScope scope;

  VTChanges* obj = ObjectWrap::Unwrap<VTChanges>(args.This());


  return scope.Close(Number::New(obj->vt->rows));
}

Handle<Value> VTChanges::Finish(const Arguments& args) {
  HandleScope scope;

  VTChanges* obj = ObjectWrap::Unwrap<VTChanges>(args.This());
#ifdef VTCHANGES_DEBUG
  fprintf(stderr, "Freeing the VT\n");
#endif
  vterm_free(obj->vt);

  return scope.Close(args.This());
}
