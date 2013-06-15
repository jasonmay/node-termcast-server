#include <stdlib.h>
#include <stdio.h>
#include <node.h>
extern "C" {
// FIXME ugh this is ugly. any way around this?
#include "../deps/libvterm/src/vterm_internal.h"
#include <vterm.h>
}

#include "vtchanges.h"

using namespace v8;

VTChanges::VTChanges() {};
VTChanges::~VTChanges() {};

Persistent<Function> VTChanges::constructor;

extern "C" {
  void ohhai() {
    void *vt = vterm_new(80, 24);
    printf("ROWS: %d\n", ((VTerm*)vt)->rows);
    vterm_free((VTerm*)vt);
  }
}

void VTChanges::Init() {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("VTChanges"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("plusOne"),
      FunctionTemplate::New(PlusOne)->GetFunction());

  ohhai();

  constructor = Persistent<Function>::New(tpl->GetFunction());
}

Handle<Value> VTChanges::New(const Arguments& args) {
  HandleScope scope;

  VTChanges* obj = new VTChanges();
  obj->counter_ = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
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

Handle<Value> VTChanges::PlusOne(const Arguments& args) {
  HandleScope scope;

  VTChanges* obj = ObjectWrap::Unwrap<VTChanges>(args.This());
  obj->counter_ += 1;

  return scope.Close(Number::New(obj->counter_));
}
