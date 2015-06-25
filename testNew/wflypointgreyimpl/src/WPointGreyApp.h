/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class WPointGreyApp */

#ifndef _Included_WPointGreyApp
#define _Included_WPointGreyApp
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     WPointGreyApp
 * Method:    initContext
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_initContext
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    connect
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_connect
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    getNumCamera
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_WPointGreyApp_getNumCamera
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    printCamera
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_printCamera
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    disconnect
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_disconnect
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    startCapture
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_startCapture
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     WPointGreyApp
 * Method:    stopCapture
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_stopCapture
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    calibrageFormat
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_calibrageFormat
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    destroysContext
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_destroysContext
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    startVideo
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_startVideo
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     WPointGreyApp
 * Method:    setVideoModeAndFrameRate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_setVideoModeAndFrameRate
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    brightness
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_brightness
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    setCustomerFrameRate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_setCustomerFrameRate
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    WBRedBlue
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_WBRedBlue
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    capture
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_WPointGreyApp_capture
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    startCamera
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_startCamera
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    getHeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_WPointGreyApp_getHeight
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    getWidth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_WPointGreyApp_getWidth
  (JNIEnv *, jobject);

/*
 * Class:     WPointGreyApp
 * Method:    getFramerate
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_WPointGreyApp_getFramerate
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif