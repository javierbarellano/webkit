list(APPEND WebCore_INCLUDE_DIRECTORIES
    "${WEBCORE_DIR}/platform/nix"
    "${WEBCORE_DIR}/platform/cairo"
    "${WEBCORE_DIR}/platform/graphics/cairo"
    "${WEBCORE_DIR}/platform/graphics/freetype"
    "${WEBCORE_DIR}/platform/graphics/glx"
    "${WEBCORE_DIR}/platform/graphics/harfbuzz/"
    "${WEBCORE_DIR}/platform/graphics/harfbuzz/ng"
    "${WEBCORE_DIR}/platform/graphics/nix"
    "${WEBCORE_DIR}/platform/graphics/opengl"
    "${WEBCORE_DIR}/platform/graphics/opentype"
    "${WEBCORE_DIR}/platform/graphics/surfaces"
    "${WEBCORE_DIR}/platform/graphics/texmap"
    "${WEBCORE_DIR}/platform/linux"
    "${WEBCORE_DIR}/platform/mediastream/gstreamer"
    "${PLATFORM_DIR}/nix/"
)

list(APPEND WebCore_SOURCES
    css/WebKitCSSArrayFunctionValue.cpp
    css/WebKitCSSMatFunctionValue.cpp
    css/WebKitCSSMatrix.cpp
    css/WebKitCSSMixFunctionValue.cpp

    editing/SmartReplaceICU.cpp

    html/shadow/MediaControlsNix.cpp

    page/TouchAdjustment.cpp

    page/nix/EventHandlerNix.cpp

    platform/ContextMenuItemNone.cpp
    platform/ContextMenuNone.cpp
    platform/Cursor.cpp
    platform/LocalizedStrings.cpp
    platform/PlatformStrategies.cpp

    platform/cairo/WidgetBackingStoreCairo.cpp

    platform/graphics/ImageSource.cpp
    platform/graphics/OpenGLShims.cpp
    platform/graphics/WOFFFileFormat.cpp

    platform/graphics/cairo/BitmapImageCairo.cpp
    platform/graphics/cairo/CairoUtilities.cpp
    platform/graphics/cairo/DrawingBufferCairo.cpp
    platform/graphics/cairo/FontCairo.cpp
    platform/graphics/cairo/FontCairoHarfbuzzNG.cpp
    platform/graphics/cairo/GradientCairo.cpp
    platform/graphics/cairo/GraphicsContextCairo.cpp
    platform/graphics/cairo/ImageBufferCairo.cpp
    platform/graphics/cairo/ImageCairo.cpp
    platform/graphics/cairo/IntRectCairo.cpp
    platform/graphics/cairo/OwnPtrCairo.cpp
    platform/graphics/cairo/PathCairo.cpp
    platform/graphics/cairo/PatternCairo.cpp
    platform/graphics/cairo/PlatformContextCairo.cpp
    platform/graphics/cairo/PlatformPathCairo.cpp
    platform/graphics/cairo/RefPtrCairo.cpp
    platform/graphics/cairo/TileCairo.cpp
    platform/graphics/cairo/TiledBackingStoreBackendCairo.cpp
    platform/graphics/cairo/TransformationMatrixCairo.cpp

    platform/graphics/filters/CustomFilterMeshGenerator.cpp
    platform/graphics/filters/CustomFilterValidatedProgram.cpp

    platform/graphics/freetype/FontCacheFreeType.cpp
    platform/graphics/freetype/FontCustomPlatformDataFreeType.cpp
    platform/graphics/freetype/FontPlatformDataFreeType.cpp
    platform/graphics/freetype/GlyphPageTreeNodeFreeType.cpp
    platform/graphics/freetype/SimpleFontDataFreeType.cpp

    platform/graphics/harfbuzz/HarfBuzzFace.cpp
    platform/graphics/harfbuzz/HarfBuzzFaceCairo.cpp
    platform/graphics/harfbuzz/HarfBuzzShaper.cpp

    platform/graphics/nix/IconNix.cpp
    platform/graphics/nix/ImageNix.cpp

    platform/graphics/opengl/Extensions3DOpenGLCommon.cpp
    platform/graphics/opengl/GraphicsContext3DOpenGLCommon.cpp

    platform/graphics/opentype/OpenTypeVerticalData.cpp

    platform/graphics/texmap/GraphicsLayerTextureMapper.cpp
    platform/graphics/texmap/TextureMapperGL.cpp
    platform/graphics/texmap/TextureMapperShaderProgram.cpp

    platform/gtk/EventLoopGtk.cpp
    platform/gtk/LoggingGtk.cpp
    platform/gtk/SharedBufferGtk.cpp

    platform/image-decoders/ImageDecoder.cpp

    platform/image-decoders/bmp/BMPImageDecoder.cpp
    platform/image-decoders/bmp/BMPImageReader.cpp

    platform/image-decoders/cairo/ImageDecoderCairo.cpp

    platform/image-decoders/gif/GIFImageDecoder.cpp
    platform/image-decoders/gif/GIFImageReader.cpp

    platform/image-decoders/ico/ICOImageDecoder.cpp

    platform/image-decoders/jpeg/JPEGImageDecoder.cpp

    platform/image-decoders/png/PNGImageDecoder.cpp

    platform/image-decoders/webp/WEBPImageDecoder.cpp

    platform/linux/GamepadDeviceLinux.cpp

    platform/nix/CursorNix.cpp
    platform/nix/DragDataNix.cpp
    platform/nix/DragImageNix.cpp
    platform/nix/GamepadsNix.cpp
    platform/nix/LanguageNix.cpp
    platform/nix/LocalizedStringsNix.cpp
    platform/nix/NixKeyboardUtilities.cpp
    platform/nix/PasteboardNix.cpp
    platform/nix/PlatformKeyboardEventNix.cpp
    platform/nix/PlatformScreenNix.cpp
    platform/nix/RenderThemeNix.cpp
    platform/nix/ScrollbarThemeNix.cpp
    platform/nix/SoundNix.cpp
    platform/nix/WidgetNix.cpp

    platform/nix/support/MultiChannelPCMData.cpp

    platform/text/LocaleNone.cpp

    platform/text/nix/TextBreakIteratorInternalICUNix.cpp

    plugins/PluginDatabase.cpp
    plugins/PluginDebug.cpp
    plugins/PluginPackage.cpp
    plugins/PluginPackageNone.cpp
    plugins/PluginStream.cpp
    plugins/PluginView.cpp
    plugins/PluginViewNone.cpp

    rendering/style/StyleCachedShader.cpp
    rendering/style/StyleCustomFilterProgram.cpp
    rendering/style/StyleCustomFilterProgramCache.cpp
)

if (WTF_USE_OPENGL_ES_2)
    list(APPEND WebCore_SOURCES
        platform/graphics/opengl/Extensions3DOpenGLES.cpp
        platform/graphics/opengl/GraphicsContext3DOpenGLES.cpp
    )
    list(INSERT WebCore_INCLUDE_DIRECTORIES 0 ${OPENGLES2_INCLUDE_DIRS})
    list(APPEND WebCore_LIBRARIES ${OPENGLES2_LIBRARIES})
    add_definitions(${OPENGLES2_DEFINITIONS})
else ()
    list(APPEND WebCore_SOURCES
        platform/graphics/opengl/Extensions3DOpenGL.cpp
        platform/graphics/opengl/GraphicsContext3DOpenGL.cpp
    )
    list(APPEND WebCore_LIBRARIES
        ${OPENGL_gl_LIBRARY}
    )
endif ()

if (WTF_USE_EGL)
    list(INSERT WebCore_INCLUDE_DIRECTORIES 0
        platform/graphics/egl
        ${EGL_INCLUDE_DIR}
    )
    list(APPEND WebCore_SOURCES
        platform/graphics/GLContext.cpp
        platform/graphics/GraphicsContext3DPrivate.cpp

        platform/graphics/cairo/GraphicsContext3DCairo.cpp

        platform/graphics/egl/GLContextFromCurrentEGL.cpp
    )
    list(APPEND WebCore_LIBRARIES ${EGL_LIBRARY})
else ()
    list(APPEND WebCore_INCLUDE_DIRECTORIES
        platform/graphics/surfaces/glx
        platform/graphics/surfaces/efl
        ${X11_X11_INCLUDE_PATH}
    )
    list(APPEND WebCore_SOURCES
        platform/graphics/efl/GraphicsContext3DEfl.cpp
        platform/graphics/efl/GraphicsContext3DPrivate.cpp

        platform/graphics/opengl/GLPlatformContext.cpp
        platform/graphics/opengl/GLPlatformSurface.cpp

        platform/graphics/surfaces/GLTransportSurface.cpp
        platform/graphics/surfaces/GraphicsSurface.cpp

        platform/graphics/surfaces/efl/GraphicsSurfaceCommon.cpp

        platform/graphics/surfaces/glx/GLXContext.cpp
        platform/graphics/surfaces/glx/GLXSurface.cpp
        platform/graphics/surfaces/glx/X11Helper.cpp
    )
    list(APPEND WebCore_LIBRARIES ${X11_X11_LIB} ${X11_Xcomposite_LIB} ${X11_Xrender_LIB})

    if (ENABLE_MEDIA_STREAM)
        list(APPEND WebCore_LIBRARIES ${X11_Xext_LIB})
    endif ()
endif ()

if (ENABLE_BATTERY_STATUS)
    list(APPEND WebCore_INCLUDE_DIRECTORIES ${DBUS_INCLUDE_DIRS})
    list(APPEND WebCore_LIBRARIES ${DBUS_LIBRARIES})
endif ()

list(APPEND WebCore_USER_AGENT_STYLE_SHEETS
    ${WEBCORE_DIR}/css/mediaControlsNix.css
    ${WEBCORE_DIR}/css/mediaControlsNixFullscreen.css
)

list(APPEND WebCore_LIBRARIES
    ${CAIRO_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${GLIB_GIO_LIBRARIES}
    ${GLIB_GMODULE_LIBRARIES}
    ${GLIB_GOBJECT_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${HARFBUZZ_LIBRARIES}
    ${JPEG_LIBRARY}
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${PNG_LIBRARY}
    ${SQLITE_LIBRARIES}
    ${ZLIB_LIBRARIES}
    Platform
)

list(APPEND WebCore_INCLUDE_DIRECTORIES
    ${FONTCONFIG_INCLUDE_DIR}
    ${CAIRO_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${LIBXSLT_INCLUDE_DIR}
    ${SQLITE_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
)

add_definitions(-DDATA_DIR="${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}")

if (ENABLE_WEB_AUDIO)
  list(APPEND WebCore_INCLUDE_DIRECTORIES
    "${WEBCORE_DIR}/platform/audio/nix"
  )
  list(APPEND WebCore_SOURCES
    platform/audio/nix/AudioBusNix.cpp
    platform/audio/nix/AudioDestinationNix.cpp
    platform/audio/nix/FFTFrameNix.cpp
  )
  set(WEB_AUDIO_DIR ${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}/webaudio/resources)
  file(GLOB WEB_AUDIO_DATA "${WEBCORE_DIR}/platform/audio/resources/*.wav")
  install(FILES ${WEB_AUDIO_DATA} DESTINATION ${WEB_AUDIO_DIR})
  add_definitions(-DUNINSTALLED_AUDIO_RESOURCES_DIR="${WEBCORE_DIR}/platform/audio/resources")
endif ()

if (WTF_USE_CURL)
    list(APPEND WebCore_INCLUDE_DIRECTORIES
        "${WEBCORE_DIR}/platform/network/curl"
    )

    list(APPEND WebCore_SOURCES
        platform/network/NetworkStorageSessionStub.cpp

        platform/network/curl/CookieDatabaseBackingStore.cpp
        platform/network/curl/CookieJarCurl.cpp
        platform/network/curl/CookieManager.cpp
        platform/network/curl/CookieNode.cpp
        platform/network/curl/CookieStorageCurl.cpp
        platform/network/curl/CredentialStorageCurl.cpp
        platform/network/curl/DNSCurl.cpp
        platform/network/curl/FormDataStreamCurl.cpp
        platform/network/curl/MultipartHandle.cpp
        platform/network/curl/ParsedCookie.cpp
        platform/network/curl/ProxyServerCurl.cpp
        platform/network/curl/ResourceHandleCurl.cpp
        platform/network/curl/ResourceHandleManager.cpp
        platform/network/curl/SocketStreamHandleCurl.cpp
        platform/network/curl/SynchronousLoaderClientCurl.cpp
    )

    list(APPEND WebCore_LIBRARIES
        ${CURL_LIBRARIES}
    )
else ()
    list(APPEND WebCore_SOURCES
        loader/soup/CachedRawResourceSoup.cpp
        loader/soup/SubresourceLoaderSoup.cpp

        platform/network/soup/AuthenticationChallengeSoup.cpp
        platform/network/soup/CookieJarSoup.cpp
        platform/network/soup/CookieStorageSoup.cpp
        platform/network/soup/CredentialStorageSoup.cpp
        platform/network/soup/DNSSoup.cpp
        platform/network/soup/GOwnPtrSoup.cpp
        platform/network/soup/NetworkStorageSessionSoup.cpp
        platform/network/soup/ProxyResolverSoup.cpp
        platform/network/soup/ProxyServerSoup.cpp
        platform/network/soup/ResourceErrorSoup.cpp
        platform/network/soup/ResourceHandleSoup.cpp
        platform/network/soup/ResourceRequestSoup.cpp
        platform/network/soup/ResourceResponseSoup.cpp
        platform/network/soup/SocketStreamHandleSoup.cpp
        platform/network/soup/SoupURIUtils.cpp
        platform/network/soup/SynchronousLoaderClientSoup.cpp

        # Uncomment it after https://bugs.webkit.org/show_bug.cgi?id=118598 is fixed and merged to Nix.
        # platform/soup/SharedBufferSoup.cpp
    )

    list(APPEND WebCore_INCLUDE_DIRECTORIES
        "${WEBCORE_DIR}/platform/network/soup"
        ${LIBSOUP_INCLUDE_DIRS}
    )

    list(APPEND WebCore_LIBRARIES
        ${LIBSOUP_LIBRARIES}
    )
endif ()
