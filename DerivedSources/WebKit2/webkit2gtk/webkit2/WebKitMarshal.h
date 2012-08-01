
#ifndef __webkit_marshal_MARSHAL_H__
#define __webkit_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* BOOLEAN:BOXED (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:1) */
extern void webkit_marshal_BOOLEAN__BOXED (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);

/* BOOLEAN:ENUM,STRING,POINTER (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:2) */
extern void webkit_marshal_BOOLEAN__ENUM_STRING_POINTER (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);

/* BOOLEAN:OBJECT (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:3) */
extern void webkit_marshal_BOOLEAN__OBJECT (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);

/* BOOLEAN:OBJECT,ENUM (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:4) */
extern void webkit_marshal_BOOLEAN__OBJECT_ENUM (GClosure     *closure,
                                                 GValue       *return_value,
                                                 guint         n_param_values,
                                                 const GValue *param_values,
                                                 gpointer      invocation_hint,
                                                 gpointer      marshal_data);

/* BOOLEAN:STRING (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:5) */
extern void webkit_marshal_BOOLEAN__STRING (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);

/* OBJECT:VOID (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:6) */
extern void webkit_marshal_OBJECT__VOID (GClosure     *closure,
                                         GValue       *return_value,
                                         guint         n_param_values,
                                         const GValue *param_values,
                                         gpointer      invocation_hint,
                                         gpointer      marshal_data);

/* VOID:OBJECT,UINT (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:7) */
extern void webkit_marshal_VOID__OBJECT_UINT (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

/* VOID:OBJECT,OBJECT (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:8) */
extern void webkit_marshal_VOID__OBJECT_OBJECT (GClosure     *closure,
                                                GValue       *return_value,
                                                guint         n_param_values,
                                                const GValue *param_values,
                                                gpointer      invocation_hint,
                                                gpointer      marshal_data);

/* VOID:OBJECT,POINTER (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:9) */
extern void webkit_marshal_VOID__OBJECT_POINTER (GClosure     *closure,
                                                 GValue       *return_value,
                                                 guint         n_param_values,
                                                 const GValue *param_values,
                                                 gpointer      invocation_hint,
                                                 gpointer      marshal_data);

/* VOID:UINT64 (./Source/WebKit2/UIProcess/API/gtk/webkit2marshal.list:10) */
extern void webkit_marshal_VOID__UINT64 (GClosure     *closure,
                                         GValue       *return_value,
                                         guint         n_param_values,
                                         const GValue *param_values,
                                         gpointer      invocation_hint,
                                         gpointer      marshal_data);

G_END_DECLS

#endif /* __webkit_marshal_MARSHAL_H__ */

