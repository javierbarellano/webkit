


#include <config.h>
#include <glib-object.h>
#include "./DerivedSources/WebKit2/webkit2gtk/webkit2/WebKitEnumTypes.h"

extern "C" {


/* Enumerations from "./Source/WebKit2/UIProcess/API/gtk/WebKitError.h". */
static const GEnumValue _webkit_network_error_values[] = {
    { WEBKIT_NETWORK_ERROR_FAILED, "WEBKIT_NETWORK_ERROR_FAILED", "failed" },
    { WEBKIT_NETWORK_ERROR_TRANSPORT, "WEBKIT_NETWORK_ERROR_TRANSPORT", "transport" },
    { WEBKIT_NETWORK_ERROR_UNKNOWN_PROTOCOL, "WEBKIT_NETWORK_ERROR_UNKNOWN_PROTOCOL", "unknown-protocol" },
    { WEBKIT_NETWORK_ERROR_CANCELLED, "WEBKIT_NETWORK_ERROR_CANCELLED", "cancelled" },
    { WEBKIT_NETWORK_ERROR_FILE_DOES_NOT_EXIST, "WEBKIT_NETWORK_ERROR_FILE_DOES_NOT_EXIST", "file-does-not-exist" },
    { 0, NULL, NULL }
};

GType webkit_network_error_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitNetworkError", _webkit_network_error_values);

    return type;
}

static const GEnumValue _webkit_policy_error_values[] = {
    { WEBKIT_POLICY_ERROR_FAILED, "WEBKIT_POLICY_ERROR_FAILED", "failed" },
    { WEBKIT_POLICY_ERROR_CANNOT_SHOW_MIME_TYPE, "WEBKIT_POLICY_ERROR_CANNOT_SHOW_MIME_TYPE", "cannot-show-mime-type" },
    { WEBKIT_POLICY_ERROR_CANNOT_SHOW_URI, "WEBKIT_POLICY_ERROR_CANNOT_SHOW_URI", "cannot-show-uri" },
    { WEBKIT_POLICY_ERROR_FRAME_LOAD_INTERRUPTED_BY_POLICY_CHANGE, "WEBKIT_POLICY_ERROR_FRAME_LOAD_INTERRUPTED_BY_POLICY_CHANGE", "frame-load-interrupted-by-policy-change" },
    { WEBKIT_POLICY_ERROR_CANNOT_USE_RESTRICTED_PORT, "WEBKIT_POLICY_ERROR_CANNOT_USE_RESTRICTED_PORT", "cannot-use-restricted-port" },
    { 0, NULL, NULL }
};

GType webkit_policy_error_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitPolicyError", _webkit_policy_error_values);

    return type;
}

static const GEnumValue _webkit_plugin_error_values[] = {
    { WEBKIT_PLUGIN_ERROR_FAILED, "WEBKIT_PLUGIN_ERROR_FAILED", "failed" },
    { WEBKIT_PLUGIN_ERROR_CANNOT_FIND_PLUGIN, "WEBKIT_PLUGIN_ERROR_CANNOT_FIND_PLUGIN", "cannot-find-plugin" },
    { WEBKIT_PLUGIN_ERROR_CANNOT_LOAD_PLUGIN, "WEBKIT_PLUGIN_ERROR_CANNOT_LOAD_PLUGIN", "cannot-load-plugin" },
    { WEBKIT_PLUGIN_ERROR_JAVA_UNAVAILABLE, "WEBKIT_PLUGIN_ERROR_JAVA_UNAVAILABLE", "java-unavailable" },
    { WEBKIT_PLUGIN_ERROR_CONNECTION_CANCELLED, "WEBKIT_PLUGIN_ERROR_CONNECTION_CANCELLED", "connection-cancelled" },
    { WEBKIT_PLUGIN_ERROR_WILL_HANDLE_LOAD, "WEBKIT_PLUGIN_ERROR_WILL_HANDLE_LOAD", "will-handle-load" },
    { 0, NULL, NULL }
};

GType webkit_plugin_error_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitPluginError", _webkit_plugin_error_values);

    return type;
}

static const GEnumValue _webkit_download_error_values[] = {
    { WEBKIT_DOWNLOAD_ERROR_NETWORK, "WEBKIT_DOWNLOAD_ERROR_NETWORK", "network" },
    { WEBKIT_DOWNLOAD_ERROR_CANCELLED_BY_USER, "WEBKIT_DOWNLOAD_ERROR_CANCELLED_BY_USER", "cancelled-by-user" },
    { WEBKIT_DOWNLOAD_ERROR_DESTINATION, "WEBKIT_DOWNLOAD_ERROR_DESTINATION", "destination" },
    { 0, NULL, NULL }
};

GType webkit_download_error_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitDownloadError", _webkit_download_error_values);

    return type;
}

static const GEnumValue _webkit_print_error_values[] = {
    { WEBKIT_PRINT_ERROR_GENERAL, "WEBKIT_PRINT_ERROR_GENERAL", "general" },
    { WEBKIT_PRINT_ERROR_PRINTER_NOT_FOUND, "WEBKIT_PRINT_ERROR_PRINTER_NOT_FOUND", "printer-not-found" },
    { WEBKIT_PRINT_ERROR_INVALID_PAGE_RANGE, "WEBKIT_PRINT_ERROR_INVALID_PAGE_RANGE", "invalid-page-range" },
    { 0, NULL, NULL }
};

GType webkit_print_error_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitPrintError", _webkit_print_error_values);

    return type;
}

static const GEnumValue _webkit_javascript_error_values[] = {
    { WEBKIT_JAVASCRIPT_ERROR_SCRIPT_FAILED, "WEBKIT_JAVASCRIPT_ERROR_SCRIPT_FAILED", "failed" },
    { 0, NULL, NULL }
};

GType webkit_javascript_error_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitJavascriptError", _webkit_javascript_error_values);

    return type;
}


/* Enumerations from "./Source/WebKit2/UIProcess/API/gtk/WebKitFindController.h". */
static const GFlagsValue _webkit_find_options_values[] = {
    { WEBKIT_FIND_OPTIONS_NONE, "WEBKIT_FIND_OPTIONS_NONE", "none" },
    { WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE, "WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE", "case-insensitive" },
    { WEBKIT_FIND_OPTIONS_AT_WORD_STARTS, "WEBKIT_FIND_OPTIONS_AT_WORD_STARTS", "at-word-starts" },
    { WEBKIT_FIND_OPTIONS_TREAT_MEDIAL_CAPITAL_AS_WORD_START, "WEBKIT_FIND_OPTIONS_TREAT_MEDIAL_CAPITAL_AS_WORD_START", "treat-medial-capital-as-word-start" },
    { WEBKIT_FIND_OPTIONS_BACKWARDS, "WEBKIT_FIND_OPTIONS_BACKWARDS", "backwards" },
    { WEBKIT_FIND_OPTIONS_WRAP_AROUND, "WEBKIT_FIND_OPTIONS_WRAP_AROUND", "wrap-around" },
    { 0, NULL, NULL }
};

GType webkit_find_options_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_flags_register_static("WebKitFindOptions", _webkit_find_options_values);

    return type;
}


/* Enumerations from "./Source/WebKit2/UIProcess/API/gtk/WebKitHitTestResult.h". */
static const GFlagsValue _webkit_hit_test_result_context_values[] = {
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_DOCUMENT, "WEBKIT_HIT_TEST_RESULT_CONTEXT_DOCUMENT", "document" },
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_LINK, "WEBKIT_HIT_TEST_RESULT_CONTEXT_LINK", "link" },
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_IMAGE, "WEBKIT_HIT_TEST_RESULT_CONTEXT_IMAGE", "image" },
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_MEDIA, "WEBKIT_HIT_TEST_RESULT_CONTEXT_MEDIA", "media" },
    { 0, NULL, NULL }
};

GType webkit_hit_test_result_context_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_flags_register_static("WebKitHitTestResultContext", _webkit_hit_test_result_context_values);

    return type;
}


/* Enumerations from "./Source/WebKit2/UIProcess/API/gtk/WebKitNavigationPolicyDecision.h". */
static const GEnumValue _webkit_navigation_type_values[] = {
    { WEBKIT_NAVIGATION_TYPE_LINK_CLICKED, "WEBKIT_NAVIGATION_TYPE_LINK_CLICKED", "link-clicked" },
    { WEBKIT_NAVIGATION_TYPE_FORM_SUBMITTED, "WEBKIT_NAVIGATION_TYPE_FORM_SUBMITTED", "form-submitted" },
    { WEBKIT_NAVIGATION_TYPE_BACK_FORWARD, "WEBKIT_NAVIGATION_TYPE_BACK_FORWARD", "back-forward" },
    { WEBKIT_NAVIGATION_TYPE_RELOAD, "WEBKIT_NAVIGATION_TYPE_RELOAD", "reload" },
    { WEBKIT_NAVIGATION_TYPE_FORM_RESUBMITTED, "WEBKIT_NAVIGATION_TYPE_FORM_RESUBMITTED", "form-resubmitted" },
    { WEBKIT_NAVIGATION_TYPE_OTHER, "WEBKIT_NAVIGATION_TYPE_OTHER", "other" },
    { 0, NULL, NULL }
};

GType webkit_navigation_type_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitNavigationType", _webkit_navigation_type_values);

    return type;
}


/* Enumerations from "./Source/WebKit2/UIProcess/API/gtk/WebKitPrintOperation.h". */
static const GEnumValue _webkit_print_operation_response_values[] = {
    { WEBKIT_PRINT_OPERATION_RESPONSE_PRINT, "WEBKIT_PRINT_OPERATION_RESPONSE_PRINT", "print" },
    { WEBKIT_PRINT_OPERATION_RESPONSE_CANCEL, "WEBKIT_PRINT_OPERATION_RESPONSE_CANCEL", "cancel" },
    { 0, NULL, NULL }
};

GType webkit_print_operation_response_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitPrintOperationResponse", _webkit_print_operation_response_values);

    return type;
}


/* Enumerations from "./Source/WebKit2/UIProcess/API/gtk/WebKitScriptDialog.h". */
static const GEnumValue _webkit_script_dialog_type_values[] = {
    { WEBKIT_SCRIPT_DIALOG_ALERT, "WEBKIT_SCRIPT_DIALOG_ALERT", "alert" },
    { WEBKIT_SCRIPT_DIALOG_CONFIRM, "WEBKIT_SCRIPT_DIALOG_CONFIRM", "confirm" },
    { WEBKIT_SCRIPT_DIALOG_PROMPT, "WEBKIT_SCRIPT_DIALOG_PROMPT", "prompt" },
    { 0, NULL, NULL }
};

GType webkit_script_dialog_type_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitScriptDialogType", _webkit_script_dialog_type_values);

    return type;
}


/* Enumerations from "./Source/WebKit2/UIProcess/API/gtk/WebKitWebContext.h". */
static const GEnumValue _webkit_cache_model_values[] = {
    { WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER, "WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER", "document-viewer" },
    { WEBKIT_CACHE_MODEL_WEB_BROWSER, "WEBKIT_CACHE_MODEL_WEB_BROWSER", "web-browser" },
    { WEBKIT_CACHE_MODEL_DOCUMENT_BROWSER, "WEBKIT_CACHE_MODEL_DOCUMENT_BROWSER", "document-browser" },
    { 0, NULL, NULL }
};

GType webkit_cache_model_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitCacheModel", _webkit_cache_model_values);

    return type;
}


/* Enumerations from "./Source/WebKit2/UIProcess/API/gtk/WebKitWebView.h". */
static const GEnumValue _webkit_policy_decision_type_values[] = {
    { WEBKIT_POLICY_DECISION_TYPE_NAVIGATION_ACTION, "WEBKIT_POLICY_DECISION_TYPE_NAVIGATION_ACTION", "navigation-action" },
    { WEBKIT_POLICY_DECISION_TYPE_NEW_WINDOW_ACTION, "WEBKIT_POLICY_DECISION_TYPE_NEW_WINDOW_ACTION", "new-window-action" },
    { WEBKIT_POLICY_DECISION_TYPE_RESPONSE, "WEBKIT_POLICY_DECISION_TYPE_RESPONSE", "response" },
    { 0, NULL, NULL }
};

GType webkit_policy_decision_type_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitPolicyDecisionType", _webkit_policy_decision_type_values);

    return type;
}

static const GEnumValue _webkit_load_event_values[] = {
    { WEBKIT_LOAD_STARTED, "WEBKIT_LOAD_STARTED", "started" },
    { WEBKIT_LOAD_REDIRECTED, "WEBKIT_LOAD_REDIRECTED", "redirected" },
    { WEBKIT_LOAD_COMMITTED, "WEBKIT_LOAD_COMMITTED", "committed" },
    { WEBKIT_LOAD_FINISHED, "WEBKIT_LOAD_FINISHED", "finished" },
    { 0, NULL, NULL }
};

GType webkit_load_event_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitLoadEvent", _webkit_load_event_values);

    return type;
}

}



