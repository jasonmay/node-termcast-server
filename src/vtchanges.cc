#include <stdlib.h>
#include <stdio.h>
#include <node.h>

#include <string>

// #include <iostream>

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
  tpl->PrototypeTemplate()->Set(String::NewSymbol("snapshot"),
      FunctionTemplate::New(Snapshot)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("resize"),
      FunctionTemplate::New(Resize)->GetFunction());
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

Handle<Value> VTChanges::Snapshot(const Arguments& args) {
  HandleScope scope;

  VTChanges* obj = ObjectWrap::Unwrap<VTChanges>(args.This());

  VTermScreen *vt_screen = vterm_obtain_screen(obj->vt);
  VTermState *vt_state = vterm_obtain_state(obj->vt);

  VTermColor default_fg, default_bg;
  vterm_state_get_default_colors(vt_state, &default_fg, &default_bg);

  int rows, cols;
  vterm_get_size(obj->vt, &rows, &cols);

  Handle<Array> changes = Array::New(0);

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {

      bool change_needed = false;

      Handle<Array> change_data = Array::New(3);
      change_data->Set(Integer::New(0), Integer::New(col));
      change_data->Set(Integer::New(1), Integer::New(row));

      VTermPos pos = {row, col};
      VTermScreenCell cell;
      vterm_screen_get_cell(vt_screen, pos, &cell);

      Handle<Object> change = Object::New();
      if (cell.width > 0 && cell.chars[0] && cell.chars[0] != 32) {

        // Skip if cell.fg == cell.bg since you won't see the value anyway!
        if (cell.fg.red != cell.bg.red ||
                cell.fg.green != cell.bg.green ||
                cell.fg.blue != cell.bg.blue) {
          Handle<Integer> foreground = Integer::New(cell.fg.red + (cell.fg.green << 8) + (cell.fg.blue << 16));
          change->Set(String::NewSymbol("fg"), foreground);

          Handle<Array> cell_value = Array::New(cell.width);
          for (int i = 0; i < cell.width; ++i) {
            cell_value->Set(Integer::New(i), Integer::New((uint32_t)cell.chars[i]));
            change->Set(String::NewSymbol("v"), cell_value);
            change_data->Set(Integer::New(2), change);
            change_needed = true;
          }
        }
      }

      if (change_needed)
        changes->Set(changes->Length(), change_data);
    }
  }

  return scope.Close(changes);
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

  /*
  char *foo = *str;
  for (size_t s = 0; s < args[0]->ToString()->Length(); ++s) {
    if (s % 10 == 0) std::cerr << std::endl;
    if (foo[s] >= 32 && foo[s] < 127)
      std::cerr << foo[s] << ":";
    else if (foo[s] == '\033')
      std::cerr << "\033[0;31me\033[m:";
    else
      std::cerr << "\033[0;32m" << (int)(uint8_t)foo[s] << "\033[m" << ":";
  }
  std::cerr << std::endl;
  */

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

      Handle<Integer> foreground = Integer::New(cell.fg.red + (cell.fg.green << 8) + (cell.fg.blue << 16));

      //std::string background_str = "";
      //background_str.push_back((char)cell.bg.red);
      //background_str.push_back((char)cell.bg.green);
      //background_str.push_back((char)cell.bg.blue);
      //Handle<String> background = String::New(background_str.c_str());

      change->Set(String::NewSymbol("fg"), foreground);
      //change->Set(String::NewSymbol("bg"), background);

      Handle<Array> cell_value = Array::New(cell.width);

      for (int i = 0; i < 6; ++i)
        cell_value->Set(Integer::New(i), Integer::New((uint32_t)cell.chars[i]));

      change->Set(String::NewSymbol("v"), cell_value);

      change_data->Set(Integer::New(2), change);
      changes->Set(changes->Length(), change_data);
    }

  }

  return scope.Close(changes);
}

Handle<Value> VTChanges::Resize(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 2 || !args[0]->IsNumber() || !args[1]->IsNumber()) {
    THROW_ERROR_EXCEPTION("Usage: .resize(rows, cols)");
  }

  VTChanges* obj = ObjectWrap::Unwrap<VTChanges>(args.This());
  vterm_set_size(obj->vt, args[0]->IntegerValue(), args[1]->IntegerValue());
  return scope.Close(Undefined());
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
