#define _CRT_SECURE_NO_WARNINGS

#include "winUtil.h"
#include "glExtensions.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "errorUtil.h"
#include <stdlib.h>

#pragma comment(lib, "opengl32")

void* getProc(const char* name) {
    void* p = wglGetProcAddress(name);
    if (p == NULL) {
        int maxLen = strlen(name) + 10;
        wchar_t* wName = calloc(maxLen, sizeof(wchar_t));
        mbstowcs(wName, name, maxLen);

        int e = GetLastError();
        fatal_windows_error(e, L"failed to get %s proc address", wName);

        free(wName);

        return NULL;
    }
    if (p == 0 ||
        (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1))
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        if (module == NULL) {
            int e = GetLastError();
            fatal_windows_error(e, L"Failed to load opengl32.dll in %s at line %i", __FILEW__, __LINE__);
            return NULL;
        }
        p = GetProcAddress(module, name);
        if (p == NULL) {
            int e = GetLastError();
            fatal_windows_error(e, L"Failed to load opengl32.dll in %s at line %i", __FILEW__, __LINE__);
            return NULL;
        }
    }

    return p;
}

void GLEInit() {
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)getProc("glCreateProgram");
    glCreateShader = (PFNGLCREATESHADERPROC)getProc("glCreateShader");

    glShaderSource = (PFNGLSHADERSOURCEPROC)getProc("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)getProc("glCompileShader");
    glAttachShader = (PFNGLATTACHSHADERPROC)getProc("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)getProc("glLinkProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)getProc("glUseProgram");

    glIsProgram = (PFNGLISPROGRAMPROC)getProc("glIsProgram");
    glIsShader = (PFNGLISSHADERPROC)getProc("glIsShader");

    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)getProc("glGetShaderInfoLog");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)getProc("glGetProgramInfoLog");

    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)getProc("glBindFramebuffer");
    glBindBuffer = (PFNGLBINDBUFFERPROC)getProc("glBindBuffer");
    glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)getProc("glBindRenderbuffer");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)getProc("glBindVertexArray");
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)getProc("glActiveTexture");

    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)getProc("glGenFramebuffers");
    glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)getProc("glGenRenderbuffers");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)getProc("glGenVertexArrays");
    glGenBuffers = (PFNGLGENBUFFERSPROC)getProc("glGenBuffers");

    glBufferData = (PFNGLBUFFERDATAPROC)getProc("glBufferData");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)getProc("glBufferSubData");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)getProc("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)getProc("glVertexAttribPointer");
    glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)getProc("glGetIntegeri_v");
    glGetStringi = (PFNGLGETSTRINGIPROC)getProc("glGetStringi");
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)getProc("wglSwapIntervalEXT");
    wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)getProc("wglGetSwapIntervalEXT");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)getProc("glUniformMatrix4fv");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)getProc("glGetUniformLocation");

    glUniform1i = (PFNGLUNIFORM1IPROC)getProc("glUniform1i");
    glUniform1f = (PFNGLUNIFORM1FPROC)getProc("glUniform1f");
    glUniform2f = (PFNGLUNIFORM2FPROC)getProc("glUniform2f");
    glUniform2i = (PFNGLUNIFORM2IPROC)getProc("glUniform2i");
    glUniform3i = (PFNGLUNIFORM3IPROC)getProc("glUniform3i");
    glUniform3f = (PFNGLUNIFORM3FPROC)getProc("glUniform3f");

    glGetShaderiv = (PFNGLGETSHADERIVPROC)getProc("glGetShaderiv");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)getProc("glGetProgramiv");

    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)getProc("glFramebufferTexture2D");
    glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)getProc("glTexStorage2D");
    glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)getProc("glTexImage2DMultisample");
    glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)getProc("glRenderbufferStorage");
    glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)getProc("glRenderbufferStorageMultisample");
    glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)getProc("glFramebufferRenderbuffer");

    glMapBuffer = (PFNGLMAPBUFFERPROC)getProc("glMapBuffer");
    glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)getProc("glUnmapBuffer");
    glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)getProc("glBlitFramebuffer");
    glBlitNamedFramebuffer = (PFNGLBLITNAMEDFRAMEBUFFERPROC)getProc("glBlitNamedFramebuffer");
    glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)getProc("glCheckFramebufferStatus");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)getProc("glGenerateMipmap");
    glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)getProc("glDrawRangeElements");

    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)getProc("glDeleteProgram");
    glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)getProc("glDeleteRenderbuffers");
    glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)getProc("glDeleteFramebuffers");
    glDeleteShader = (PFNGLDELETESHADERPROC)getProc("glDeleteShader");
    glDetachShader = (PFNGLDETACHSHADERPROC)getProc("glDetachShader");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)getProc("glDeleteBuffers");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)getProc("glDeleteVertexArrays");

    initialized = 1;
}
