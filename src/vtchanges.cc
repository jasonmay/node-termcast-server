#include <stdlib.h>
#include <stdio.h>
#include <node.h>

#include <string>

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

static int vt_damage(VTermRect rect, void *user) {
  Handle<Object> *cdata = reinterpret_cast<Handle<Object>*>(user);
  if ((*cdata)->Get(String::NewSymbol("cells"))->IsUndefined()) {
    (*cdata)->Set(String::NewSymbol("cells"), Array::New(0));
  }

#define VT_CELLS_ARRAY (Handle<Array>::Cast( \
                          (*cdata)->Get(String::NewSymbol("cells")) \
                        ))

  int col, row;
  for (row = rect.start_row; row < rect.end_row; ++row) {
    if (VT_CELLS_ARRAY->Get(Integer::New(row))->IsUndefined())
      VT_CELLS_ARRAY->Set(Integer::New(row), Array::New(0));

    for (col = rect.start_col; col < rect.end_col; ++col) {
      Local<Object> cell_data = Object::New();
      Handle<Array>::Cast(VT_CELLS_ARRAY->Get(Integer::New(row)))->Set(Integer::New(col), cell_data);
    }
  }

#undef VT_CELLS_ARRAY
  return 1;
}

static int vt_resize(int rows, int cols, void *user) {
  Handle<Object> *cdata = reinterpret_cast<Handle<Object>*>(user);

  if ((*cdata)->Get(String::NewSymbol("resize"))->IsUndefined()) {
    (*cdata)->Set(String::NewSymbol("resize"), Array::New(2));
  }
  Handle<Array>::Cast((*cdata)->Get(String::NewSymbol("resize")))->Set(Integer::New(0), Integer::New(rows));
  Handle<Array>::Cast((*cdata)->Get(String::NewSymbol("resize")))->Set(Integer::New(1), Integer::New(cols));

  return 1;
}

static VTermScreenCallbacks screen_cbs = {
  &vt_damage, NULL, NULL, NULL, NULL, NULL, &vt_resize, NULL, NULL
};

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

  Handle<Object> cdata = Object::New();
  VTermScreen *vt_screen = vterm_obtain_screen(obj->vt);

  vterm_screen_set_callbacks(vt_screen, &screen_cbs, &cdata);
  vterm_push_bytes(obj->vt, *str, args[0]->ToString()->Length());

  Handle<Array> changes = Array::New(0);

  Handle<Array> cells = Handle<Array>::Cast(cdata->Get(String::NewSymbol("cells")));
  if (cells->IsUndefined()) cells = Array::New(0);

  // TODO use GetPropertyNames and loop through that instead (perf)
  for (uint32_t row = 0; row < cells->Length(); ++row) {
    if (cells->Get(Integer::New(row))->IsUndefined())
      continue;

    Handle<Array> row_array = Handle<Array>::Cast(cells->Get(Integer::New(row)));
   for (uint32_t col = 0; col < row_array->Length(); ++col) {
     if (row_array->Get(Integer::New(col))->IsUndefined())
       continue;

      Handle<Object> change = Handle<Object>::Cast(row_array->Get(Integer::New(col)));

      Handle<Array> change_data = Array::New(3);
      change_data->Set(Integer::New(0), Integer::New(col));
      change_data->Set(Integer::New(1), Integer::New(row));

      VTermScreen *vt_screen = vterm_obtain_screen(obj->vt);

      VTermPos pos = {row, col};
      VTermScreenCell cell;
      vterm_screen_get_cell(vt_screen, pos, &cell);

      //fprintf(stderr, "Value: %d at r%d c%d\n", cell.chars[0], pos.row, pos.col);
      std::string foreground_str = "";
      foreground_str.push_back((char)cell.fg.red);
      foreground_str.push_back((char)cell.fg.green);
      foreground_str.push_back((char)cell.fg.blue);
      Handle<String> foreground = String::NewSymbol(foreground_str.c_str());

      std::string background_str = "";
      background_str.push_back((char)cell.fg.red);
      background_str.push_back((char)cell.fg.green);
      background_str.push_back((char)cell.fg.blue);
      Handle<String> background = String::NewSymbol(background_str.c_str());

      change->Set(String::NewSymbol("fg"), foreground);
      change->Set(String::NewSymbol("bg"), background);

      Handle<Array> cell_value = Array::New(cell.width);
      for (int i = 0; i < cell.width; ++i)
        cell_value->Set(Integer::New(i), Integer::New(cell.chars[i]));
      change->Set(String::NewSymbol("v"), cell_value);

      change_data->Set(Integer::New(2), change);
      changes->Set(changes->Length(), change_data);
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
