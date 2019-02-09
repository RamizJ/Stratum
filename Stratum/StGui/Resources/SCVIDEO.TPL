// Stratum Computer Video functions
// Based on MS Video for WINDOWS

name "OpenVideo"           arg "STRING","FLOAT" ret "HANDLE" out 441
name "CloseVideo"          arg "HANDLE" ret "FLOAT" out 442
name "CreateVideoFrame2d"  arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT","FLOAT","FLOAT" ret "HANDLE" out 446
name "VideoSetPos2d"       arg "HANDLE","FLOAT" ret "FLOAT" out 447
name "FrameSetPos2d"       arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 448
name "VideoPlay2d"         arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 449
name "VideoPause2d"        arg "HANDLE","HANDLE" ret "FLOAT" out 450
name "VideoResume2d"       arg "HANDLE","HANDLE" ret "FLOAT" out 451
name "VideoStop2d"         arg "HANDLE","HANDLE" ret "FLOAT" out 452
name "FrameSetSrcRect2d"   arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 453
name "VideoGetPos2d"       arg "HANDLE" ret "FLOAT" out 454
name "FrameGetPos2d"       arg "HANDLE","HANDLE" ret "FLOAT" out 455
name "FrameGetVideo2d"     arg "HANDLE","HANDLE" ret "HANDLE" out 456
name "BeginWriteVideo2d"   arg "HANDLE","STRING","FLOAT","FLOAT","FLOAT","FLOAT","STRING" ret "HANDLE" out 457
name "WriteVideoFrame2d"   arg "HANDLE" ret "FLOAT" out 459
name "VideoDialog" arg "HANDLE" ret "FLOAT" out 458
name "GetVideoMarker"      arg "HANDLE","COLORREF","COLORREF","COLORREF",&"FLOAT",&"FLOAT" ret "FLOAT" out 749

