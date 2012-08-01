


#include <config.h>
#include <glib-object.h>
#include "./DerivedSources/webkit/webkitenumtypes.h"

extern "C" {


/* enumerations from "./Source/WebKit/gtk/webkit/webkitdownload.h" */
static const GEnumValue _webkit_download_status_values[] = {
    { WEBKIT_DOWNLOAD_STATUS_ERROR, "WEBKIT_DOWNLOAD_STATUS_ERROR", "error" },
    { WEBKIT_DOWNLOAD_STATUS_CREATED, "WEBKIT_DOWNLOAD_STATUS_CREATED", "created" },
    { WEBKIT_DOWNLOAD_STATUS_STARTED, "WEBKIT_DOWNLOAD_STATUS_STARTED", "started" },
    { WEBKIT_DOWNLOAD_STATUS_CANCELLED, "WEBKIT_DOWNLOAD_STATUS_CANCELLED", "cancelled" },
    { WEBKIT_DOWNLOAD_STATUS_FINISHED, "WEBKIT_DOWNLOAD_STATUS_FINISHED", "finished" },
    { 0, NULL, NULL }
};

GType webkit_download_status_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitDownloadStatus", _webkit_download_status_values);

    return type;
}

static const GEnumValue _webkit_download_error_values[] = {
    { WEBKIT_DOWNLOAD_ERROR_CANCELLED_BY_USER, "WEBKIT_DOWNLOAD_ERROR_CANCELLED_BY_USER", "cancelled-by-user" },
    { WEBKIT_DOWNLOAD_ERROR_DESTINATION, "WEBKIT_DOWNLOAD_ERROR_DESTINATION", "destination" },
    { WEBKIT_DOWNLOAD_ERROR_NETWORK, "WEBKIT_DOWNLOAD_ERROR_NETWORK", "network" },
    { 0, NULL, NULL }
};

GType webkit_download_error_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitDownloadError", _webkit_download_error_values);

    return type;
}


/* enumerations from "./Source/WebKit/gtk/webkit/webkiterror.h" */
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
    { WEBKIT_POLICY_ERROR_CANNOT_SHOW_URL, "WEBKIT_POLICY_ERROR_CANNOT_SHOW_URL", "cannot-show-url" },
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


/* enumerations from "./Source/WebKit/gtk/webkit/webkitglobals.h" */
static const GEnumValue _webkit_cache_model_values[] = {
    { WEBKIT_CACHE_MODEL_DEFAULT, "WEBKIT_CACHE_MODEL_DEFAULT", "default" },
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

static const GEnumValue _webkit_context_menu_action_values[] = {
    { WEBKIT_CONTEXT_MENU_ACTION_NO_ACTION, "WEBKIT_CONTEXT_MENU_ACTION_NO_ACTION", "no-action" },
    { WEBKIT_CONTEXT_MENU_ACTION_OPEN_LINK, "WEBKIT_CONTEXT_MENU_ACTION_OPEN_LINK", "open-link" },
    { WEBKIT_CONTEXT_MENU_ACTION_OPEN_LINK_IN_NEW_WINDOW, "WEBKIT_CONTEXT_MENU_ACTION_OPEN_LINK_IN_NEW_WINDOW", "open-link-in-new-window" },
    { WEBKIT_CONTEXT_MENU_ACTION_DOWNLOAD_LINK_TO_DISK, "WEBKIT_CONTEXT_MENU_ACTION_DOWNLOAD_LINK_TO_DISK", "download-link-to-disk" },
    { WEBKIT_CONTEXT_MENU_ACTION_COPY_LINK_TO_CLIPBOARD, "WEBKIT_CONTEXT_MENU_ACTION_COPY_LINK_TO_CLIPBOARD", "copy-link-to-clipboard" },
    { WEBKIT_CONTEXT_MENU_ACTION_OPEN_IMAGE_IN_NEW_WINDOW, "WEBKIT_CONTEXT_MENU_ACTION_OPEN_IMAGE_IN_NEW_WINDOW", "open-image-in-new-window" },
    { WEBKIT_CONTEXT_MENU_ACTION_DOWNLOAD_IMAGE_TO_DISK, "WEBKIT_CONTEXT_MENU_ACTION_DOWNLOAD_IMAGE_TO_DISK", "download-image-to-disk" },
    { WEBKIT_CONTEXT_MENU_ACTION_COPY_IMAGE_TO_CLIPBOARD, "WEBKIT_CONTEXT_MENU_ACTION_COPY_IMAGE_TO_CLIPBOARD", "copy-image-to-clipboard" },
    { WEBKIT_CONTEXT_MENU_ACTION_COPY_IMAGE_URL_TO_CLIPBOARD, "WEBKIT_CONTEXT_MENU_ACTION_COPY_IMAGE_URL_TO_CLIPBOARD", "copy-image-url-to-clipboard" },
    { WEBKIT_CONTEXT_MENU_ACTION_OPEN_FRAME_IN_NEW_WINDOW, "WEBKIT_CONTEXT_MENU_ACTION_OPEN_FRAME_IN_NEW_WINDOW", "open-frame-in-new-window" },
    { WEBKIT_CONTEXT_MENU_ACTION_GO_BACK, "WEBKIT_CONTEXT_MENU_ACTION_GO_BACK", "go-back" },
    { WEBKIT_CONTEXT_MENU_ACTION_GO_FORWARD, "WEBKIT_CONTEXT_MENU_ACTION_GO_FORWARD", "go-forward" },
    { WEBKIT_CONTEXT_MENU_ACTION_STOP, "WEBKIT_CONTEXT_MENU_ACTION_STOP", "stop" },
    { WEBKIT_CONTEXT_MENU_ACTION_RELOAD, "WEBKIT_CONTEXT_MENU_ACTION_RELOAD", "reload" },
    { WEBKIT_CONTEXT_MENU_ACTION_COPY, "WEBKIT_CONTEXT_MENU_ACTION_COPY", "copy" },
    { WEBKIT_CONTEXT_MENU_ACTION_CUT, "WEBKIT_CONTEXT_MENU_ACTION_CUT", "cut" },
    { WEBKIT_CONTEXT_MENU_ACTION_PASTE, "WEBKIT_CONTEXT_MENU_ACTION_PASTE", "paste" },
    { WEBKIT_CONTEXT_MENU_ACTION_DELETE, "WEBKIT_CONTEXT_MENU_ACTION_DELETE", "delete" },
    { WEBKIT_CONTEXT_MENU_ACTION_SELECT_ALL, "WEBKIT_CONTEXT_MENU_ACTION_SELECT_ALL", "select-all" },
    { WEBKIT_CONTEXT_MENU_ACTION_INPUT_METHODS, "WEBKIT_CONTEXT_MENU_ACTION_INPUT_METHODS", "input-methods" },
    { WEBKIT_CONTEXT_MENU_ACTION_UNICODE, "WEBKIT_CONTEXT_MENU_ACTION_UNICODE", "unicode" },
    { WEBKIT_CONTEXT_MENU_ACTION_SPELLING_GUESS, "WEBKIT_CONTEXT_MENU_ACTION_SPELLING_GUESS", "spelling-guess" },
    { WEBKIT_CONTEXT_MENU_ACTION_NO_GUESSES_FOUND, "WEBKIT_CONTEXT_MENU_ACTION_NO_GUESSES_FOUND", "no-guesses-found" },
    { WEBKIT_CONTEXT_MENU_ACTION_IGNORE_SPELLING, "WEBKIT_CONTEXT_MENU_ACTION_IGNORE_SPELLING", "ignore-spelling" },
    { WEBKIT_CONTEXT_MENU_ACTION_LEARN_SPELLING, "WEBKIT_CONTEXT_MENU_ACTION_LEARN_SPELLING", "learn-spelling" },
    { WEBKIT_CONTEXT_MENU_ACTION_IGNORE_GRAMMAR, "WEBKIT_CONTEXT_MENU_ACTION_IGNORE_GRAMMAR", "ignore-grammar" },
    { WEBKIT_CONTEXT_MENU_ACTION_FONT_MENU, "WEBKIT_CONTEXT_MENU_ACTION_FONT_MENU", "font-menu" },
    { WEBKIT_CONTEXT_MENU_ACTION_BOLD, "WEBKIT_CONTEXT_MENU_ACTION_BOLD", "bold" },
    { WEBKIT_CONTEXT_MENU_ACTION_ITALIC, "WEBKIT_CONTEXT_MENU_ACTION_ITALIC", "italic" },
    { WEBKIT_CONTEXT_MENU_ACTION_UNDERLINE, "WEBKIT_CONTEXT_MENU_ACTION_UNDERLINE", "underline" },
    { WEBKIT_CONTEXT_MENU_ACTION_OUTLINE, "WEBKIT_CONTEXT_MENU_ACTION_OUTLINE", "outline" },
    { WEBKIT_CONTEXT_MENU_ACTION_INSPECT_ELEMENT, "WEBKIT_CONTEXT_MENU_ACTION_INSPECT_ELEMENT", "inspect-element" },
    { WEBKIT_CONTEXT_MENU_ACTION_OPEN_MEDIA_IN_NEW_WINDOW, "WEBKIT_CONTEXT_MENU_ACTION_OPEN_MEDIA_IN_NEW_WINDOW", "open-media-in-new-window" },
    { WEBKIT_CONTEXT_MENU_ACTION_COPY_MEDIA_LINK_TO_CLIPBOARD, "WEBKIT_CONTEXT_MENU_ACTION_COPY_MEDIA_LINK_TO_CLIPBOARD", "copy-media-link-to-clipboard" },
    { WEBKIT_CONTEXT_MENU_ACTION_TOGGLE_MEDIA_CONTROLS, "WEBKIT_CONTEXT_MENU_ACTION_TOGGLE_MEDIA_CONTROLS", "toggle-media-controls" },
    { WEBKIT_CONTEXT_MENU_ACTION_TOGGLE_MEDIA_LOOP, "WEBKIT_CONTEXT_MENU_ACTION_TOGGLE_MEDIA_LOOP", "toggle-media-loop" },
    { WEBKIT_CONTEXT_MENU_ACTION_ENTER_VIDEO_FULLSCREEN, "WEBKIT_CONTEXT_MENU_ACTION_ENTER_VIDEO_FULLSCREEN", "enter-video-fullscreen" },
    { WEBKIT_CONTEXT_MENU_ACTION_MEDIA_PLAY_PAUSE, "WEBKIT_CONTEXT_MENU_ACTION_MEDIA_PLAY_PAUSE", "media-play-pause" },
    { WEBKIT_CONTEXT_MENU_ACTION_MEDIA_MUTE, "WEBKIT_CONTEXT_MENU_ACTION_MEDIA_MUTE", "media-mute" },
    { 0, NULL, NULL }
};

GType webkit_context_menu_action_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitContextMenuAction", _webkit_context_menu_action_values);

    return type;
}


/* enumerations from "./Source/WebKit/gtk/webkit/webkithittestresult.h" */
static const GFlagsValue _webkit_hit_test_result_context_values[] = {
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_DOCUMENT, "WEBKIT_HIT_TEST_RESULT_CONTEXT_DOCUMENT", "document" },
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_LINK, "WEBKIT_HIT_TEST_RESULT_CONTEXT_LINK", "link" },
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_IMAGE, "WEBKIT_HIT_TEST_RESULT_CONTEXT_IMAGE", "image" },
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_MEDIA, "WEBKIT_HIT_TEST_RESULT_CONTEXT_MEDIA", "media" },
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_SELECTION, "WEBKIT_HIT_TEST_RESULT_CONTEXT_SELECTION", "selection" },
    { WEBKIT_HIT_TEST_RESULT_CONTEXT_EDITABLE, "WEBKIT_HIT_TEST_RESULT_CONTEXT_EDITABLE", "editable" },
    { 0, NULL, NULL }
};

GType webkit_hit_test_result_context_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_flags_register_static("WebKitHitTestResultContext", _webkit_hit_test_result_context_values);

    return type;
}


/* enumerations from "./Source/WebKit/gtk/webkit/webkitwebframe.h" */
static const GEnumValue _webkit_load_status_values[] = {
    { WEBKIT_LOAD_PROVISIONAL, "WEBKIT_LOAD_PROVISIONAL", "provisional" },
    { WEBKIT_LOAD_COMMITTED, "WEBKIT_LOAD_COMMITTED", "committed" },
    { WEBKIT_LOAD_FINISHED, "WEBKIT_LOAD_FINISHED", "finished" },
    { WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT, "WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT", "first-visually-non-empty-layout" },
    { WEBKIT_LOAD_FAILED, "WEBKIT_LOAD_FAILED", "failed" },
    { 0, NULL, NULL }
};

GType webkit_load_status_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitLoadStatus", _webkit_load_status_values);

    return type;
}


/* enumerations from "./Source/WebKit/gtk/webkit/webkitwebnavigationaction.h" */
static const GEnumValue _webkit_web_navigation_reason_values[] = {
    { WEBKIT_WEB_NAVIGATION_REASON_LINK_CLICKED, "WEBKIT_WEB_NAVIGATION_REASON_LINK_CLICKED", "link-clicked" },
    { WEBKIT_WEB_NAVIGATION_REASON_FORM_SUBMITTED, "WEBKIT_WEB_NAVIGATION_REASON_FORM_SUBMITTED", "form-submitted" },
    { WEBKIT_WEB_NAVIGATION_REASON_BACK_FORWARD, "WEBKIT_WEB_NAVIGATION_REASON_BACK_FORWARD", "back-forward" },
    { WEBKIT_WEB_NAVIGATION_REASON_RELOAD, "WEBKIT_WEB_NAVIGATION_REASON_RELOAD", "reload" },
    { WEBKIT_WEB_NAVIGATION_REASON_FORM_RESUBMITTED, "WEBKIT_WEB_NAVIGATION_REASON_FORM_RESUBMITTED", "form-resubmitted" },
    { WEBKIT_WEB_NAVIGATION_REASON_OTHER, "WEBKIT_WEB_NAVIGATION_REASON_OTHER", "other" },
    { 0, NULL, NULL }
};

GType webkit_web_navigation_reason_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitWebNavigationReason", _webkit_web_navigation_reason_values);

    return type;
}


/* enumerations from "./Source/WebKit/gtk/webkit/webkitwebsettings.h" */
static const GEnumValue _webkit_editing_behavior_values[] = {
    { WEBKIT_EDITING_BEHAVIOR_MAC, "WEBKIT_EDITING_BEHAVIOR_MAC", "mac" },
    { WEBKIT_EDITING_BEHAVIOR_WINDOWS, "WEBKIT_EDITING_BEHAVIOR_WINDOWS", "windows" },
    { WEBKIT_EDITING_BEHAVIOR_UNIX, "WEBKIT_EDITING_BEHAVIOR_UNIX", "unix" },
    { 0, NULL, NULL }
};

GType webkit_editing_behavior_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitEditingBehavior", _webkit_editing_behavior_values);

    return type;
}


/* enumerations from "./Source/WebKit/gtk/webkit/webkitwebview.h" */
static const GEnumValue _webkit_navigation_response_values[] = {
    { WEBKIT_NAVIGATION_RESPONSE_ACCEPT, "WEBKIT_NAVIGATION_RESPONSE_ACCEPT", "accept" },
    { WEBKIT_NAVIGATION_RESPONSE_IGNORE, "WEBKIT_NAVIGATION_RESPONSE_IGNORE", "ignore" },
    { WEBKIT_NAVIGATION_RESPONSE_DOWNLOAD, "WEBKIT_NAVIGATION_RESPONSE_DOWNLOAD", "download" },
    { 0, NULL, NULL }
};

GType webkit_navigation_response_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitNavigationResponse", _webkit_navigation_response_values);

    return type;
}

static const GEnumValue _webkit_web_view_target_info_values[] = {
    { WEBKIT_WEB_VIEW_TARGET_INFO_HTML, "WEBKIT_WEB_VIEW_TARGET_INFO_HTML", "html" },
    { WEBKIT_WEB_VIEW_TARGET_INFO_TEXT, "WEBKIT_WEB_VIEW_TARGET_INFO_TEXT", "text" },
    { WEBKIT_WEB_VIEW_TARGET_INFO_IMAGE, "WEBKIT_WEB_VIEW_TARGET_INFO_IMAGE", "image" },
    { WEBKIT_WEB_VIEW_TARGET_INFO_URI_LIST, "WEBKIT_WEB_VIEW_TARGET_INFO_URI_LIST", "uri-list" },
    { WEBKIT_WEB_VIEW_TARGET_INFO_NETSCAPE_URL, "WEBKIT_WEB_VIEW_TARGET_INFO_NETSCAPE_URL", "netscape-url" },
    { 0, NULL, NULL }
};

GType webkit_web_view_target_info_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitWebViewTargetInfo", _webkit_web_view_target_info_values);

    return type;
}

static const GEnumValue _webkit_web_view_view_mode_values[] = {
    { WEBKIT_WEB_VIEW_VIEW_MODE_WINDOWED, "WEBKIT_WEB_VIEW_VIEW_MODE_WINDOWED", "windowed" },
    { WEBKIT_WEB_VIEW_VIEW_MODE_FLOATING, "WEBKIT_WEB_VIEW_VIEW_MODE_FLOATING", "floating" },
    { WEBKIT_WEB_VIEW_VIEW_MODE_FULLSCREEN, "WEBKIT_WEB_VIEW_VIEW_MODE_FULLSCREEN", "fullscreen" },
    { WEBKIT_WEB_VIEW_VIEW_MODE_MAXIMIZED, "WEBKIT_WEB_VIEW_VIEW_MODE_MAXIMIZED", "maximized" },
    { WEBKIT_WEB_VIEW_VIEW_MODE_MINIMIZED, "WEBKIT_WEB_VIEW_VIEW_MODE_MINIMIZED", "minimized" },
    { 0, NULL, NULL }
};

GType webkit_web_view_view_mode_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitWebViewViewMode", _webkit_web_view_view_mode_values);

    return type;
}

static const GEnumValue _webkit_selection_affinity_values[] = {
    { WEBKIT_SELECTION_AFFINITY_UPSTREAM, "WEBKIT_SELECTION_AFFINITY_UPSTREAM", "upstream" },
    { WEBKIT_SELECTION_AFFINITY_DOWNSTREAM, "WEBKIT_SELECTION_AFFINITY_DOWNSTREAM", "downstream" },
    { 0, NULL, NULL }
};

GType webkit_selection_affinity_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitSelectionAffinity", _webkit_selection_affinity_values);

    return type;
}

static const GEnumValue _webkit_insert_action_values[] = {
    { WEBKIT_INSERT_ACTION_TYPED, "WEBKIT_INSERT_ACTION_TYPED", "typed" },
    { WEBKIT_INSERT_ACTION_PASTED, "WEBKIT_INSERT_ACTION_PASTED", "pasted" },
    { WEBKIT_INSERT_ACTION_DROPPED, "WEBKIT_INSERT_ACTION_DROPPED", "dropped" },
    { 0, NULL, NULL }
};

GType webkit_insert_action_get_type(void)
{
    static GType type = 0;

    if (!type)
        type = g_enum_register_static("WebKitInsertAction", _webkit_insert_action_values);

    return type;
}

}



