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

static int putglyph_cb(VTermGlyphInfo *info, VTermPos pos, void *user) {
  Handle<Array> *list = reinterpret_cast< Handle<Array> *>(user);

  if ((*list)->Get(Integer::New(pos.row))->IsUndefined())
    (*list)->Set(Integer::New(pos.row), Array::New(0));

  Local<Object> cell_data = Object::New();

  Handle<Array> value_list = Array::New(info->width);
  for (int cn = 0; cn < info->width; ++cn)
    value_list->Set(Integer::New(cn), Integer::New(info->chars[cn]));

  cell_data->Set(String::New("value"), value_list);

  Handle<Array>::Cast((*list)->Get(Integer::New(pos.row)))->Set(Integer::New(pos.col), cell_data);

  return 1;
}

static VTermStateCallbacks state_cbs = { &putglyph_cb,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

Handle<Value> VTChanges::New(const Arguments& args) {
  HandleScope scope;

  VTChanges* obj = new VTChanges();
#ifdef VTCHANGES_DEBUG
  fprintf(stderr, "Allocating a VT");
#endif
  obj->vt = vterm_new(args[0]->Uint32Value(), args[1]->Uint32Value());
  VTermScreen *vt_screen = vterm_obtain_screen(obj->vt);
  vterm_screen_reset(vt_screen, 1);
  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> VTChanges::NewInstance(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 2 &&
      !(args[0]->IsInt32() && args[1]->IsInt32())) {
    THROW_ERROR_EXCEPTION("VTChanges requires two int-sized values");
  }

  const unsigned argc = 2;
  Handle<Value> argv[argc] = { args[0], args[1] };
  Local<Object> instance = constructor->NewInstance(argc, argv);

  return scope.Close(instance);
}

Handle<Value> VTChanges::Process(const Arguments& args) {
  HandleScope scope;

  VTChanges* obj = ObjectWrap::Unwrap<VTChanges>(args.This());

  if (args.Length() < 1 || !args[0]->IsString()) {
    THROW_ERROR_EXCEPTION("the process method takes a string argument");
  }
  String::Utf8Value str(args[0]);

  Handle<Array> list = Array::New(0);
  VTermState *vt_state = vterm_obtain_state(obj->vt);
  vterm_state_set_callbacks(vt_state, &state_cbs, &list);
  vterm_push_bytes(obj->vt, *str, args[0]->ToString()->Length());

  Handle<Array> changes = Array::New(0);

  for (uint32_t row = 0; row < list->Length(); ++row) {
    if (list->Get(Integer::New(row))->IsUndefined())
      continue;

    Handle<Array> row_array = Handle<Array>::Cast(list->Get(Integer::New(row)));
    for (uint32_t col = 0; col < row_array->Length(); ++col) {
      if (row_array->Get(Integer::New(col))->IsUndefined())
        continue;

      Handle<Object> change = Handle<Object>::Cast(row_array->Get(Integer::New(col)));

      Handle<Array> change_data = Array::New(3);
      change_data->Set(Integer::New(0), Integer::New(col));
      change_data->Set(Integer::New(1), Integer::New(row));
      change_data->Set(Integer::New(2), change);
      changes->Set(changes->Length() + 1, change_data);
    }

  }

  return scope.Close(changes);
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
