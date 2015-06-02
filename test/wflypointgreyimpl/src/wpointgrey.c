#include <stdio.h>
#include <stdlib.h>


//notre point h
#include "WPointGreyApp.h"

//inclusion des entetes de pointgray

#include  "C/FlyCapture2_C.h"

// definition des constantes
#define WHITEBALANCE 3
#define BRIGHTNESS 0

typedef enum _AviType
{
    UNCOMPRESSED,
    MJPG,
    H264
} AviType;

//variables globales
fc2Context pContext ;
fc2PGRGuid pGuid ;
fc2AVIContext pAVIContext ;

/*
    - luminosité :  [0..255]
	 W.B red   : [0..1023] , avec un booléen on/off, on == active , off = desactive
     W.B blue   : [0..1023]
	- pour que ça marche, il ne faut pas que la camera soit dans le mode absolute
*/
	

void SetTimeStamping( fc2Context context, BOOL enableTimeStamp )
{
    fc2Error error;
    fc2EmbeddedImageInfo embeddedInfo;
            //exemple d'apple du fonction de la dll   
    error = fc2GetEmbeddedImageInfo( context, &embeddedInfo );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2GetEmbeddedImageInfo: %s\n", fc2ErrorToDescription( error) );
    }

    if ( embeddedInfo.timestamp.available != 0 )
    {       
        embeddedInfo.timestamp.onOff = enableTimeStamp;
    }    

    error = fc2SetEmbeddedImageInfo( context, &embeddedInfo );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2SetEmbeddedImageInfo: %s\n", fc2ErrorToDescription( error) );
    }
}


void GrabImages( fc2Context context, int numImagesToGrab, const char * fileName )
{
    fc2Error error;
    fc2Image rawImage;
    fc2Image convertedImage;
    fc2TimeStamp prevTimestamp = {0};
    int i;

    error = fc2CreateImage( &rawImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2CreateImage: %s\n", fc2ErrorToDescription( error) );
    }

    error = fc2CreateImage( &convertedImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2CreateImage: %s\n", fc2ErrorToDescription( error) );
    }

    // If externally allocated memory is to be used for the converted image,
    // simply assigning the pData member of the fc2Image structure is
    // insufficient. fc2SetImageData() should be called in order to populate
    // the fc2Image structure correctly. This can be done at this point,
    // assuming that the memory has already been allocated.

    for ( i=0; i < numImagesToGrab; i++ )
    {
        // Retrieve the image
        error = fc2RetrieveBuffer( context, &rawImage );
        if ( error != FC2_ERROR_OK )
        {
            printf( "Error in retrieveBuffer: %s\n", fc2ErrorToDescription( error));
        }
        else
        {
            // Get and print out the time stamp
            fc2TimeStamp ts = fc2GetImageTimeStamp( &rawImage);
            int diff = (ts.cycleSeconds - prevTimestamp.cycleSeconds) * 8000
                        + (ts.cycleCount - prevTimestamp.cycleCount);
            prevTimestamp = ts;
            printf( 
                "timestamp [%d %d] - %d\n", 
                ts.cycleSeconds, 
                ts.cycleCount, 
                diff );
        }       
    }

    if ( error == FC2_ERROR_OK )
    {
        // Convert the final image to RGB
        error = fc2ConvertImageTo(FC2_PIXEL_FORMAT_BGR, &rawImage, &convertedImage);
        if ( error != FC2_ERROR_OK )
        {
            printf( "Error in fc2ConvertImageTo: %s\n", fc2ErrorToDescription( error) );
        }

        // Save it to PNG
        printf("Saving the last image to %s  \n",fileName);  
		error = fc2SaveImage( &convertedImage, fileName, FC2_PNG );
		if ( error != FC2_ERROR_OK )
		{
			printf( "Error in fc2SaveImage: %s\n", fc2ErrorToDescription( error) );
			printf( "Please check write permissions.\n");
		}
    }

    error = fc2DestroyImage( &rawImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2DestroyImage: %s\n", fc2ErrorToDescription( error) );
    }

    error = fc2DestroyImage( &convertedImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2DestroyImage: %s\n", fc2ErrorToDescription( error) );
    }
}
float GetFrameRate(fc2Context context)
{
    fc2Error error;
    fc2PropertyInfo propInfo;
    fc2Property prop;

    // Check if the camera supports the FRAME_RATE property
    printf( "Detecting frame rate from camera... \n" );
    propInfo.type = FC2_FRAME_RATE;
    error = fc2GetPropertyInfo(context, &propInfo);
    if (error != FC2_ERROR_OK)
    {
        return 0.0f;
    }
    
    if (propInfo.present)
    {
        // Get the frame rate
        prop.type = FC2_FRAME_RATE;
        error = fc2GetProperty(context, &prop);
        if (error != FC2_ERROR_OK)
        {
            return 0.0f;
        }
        return prop.absValue;        
    }

    return 0.0f;
}
int SaveAVIHelper(fc2Context context, AviType aviType, float frameRate, int count, const char* fileName)
{
    fc2Error error;
    fc2Image rawImage;
    fc2AVIContext aviContext;
    fc2AVIOption aviOption;
    fc2H264Option h264Option;
    fc2MJPGOption mjpgOption;
    int i;

    error = fc2CreateAVI(&aviContext);
    if (error != FC2_ERROR_OK)
    {
        printf("Error in fc2CreateAVI: %d\n", error);
        return -1;
    }

    error = fc2CreateImage( &rawImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2CreateImage: %d\n", error );           
        fc2DestroyAVI(aviContext);
        return -1;
    }

    for (i=0; i < count; i++)
    {
        // Retrieve the image
        error = fc2RetrieveBuffer(context, &rawImage);
        if (error != FC2_ERROR_OK)
        {
            printf("Error in retrieveBuffer: %d\n", error);
            continue;
        }

        // Perform some initialization for the first time
        if (i == 0)
        {
            switch (aviType)
            {
            case H264:     
                h264Option.frameRate = frameRate;
                h264Option.bitrate = 1000000;
                h264Option.width = rawImage.cols;
                h264Option.height = rawImage.rows;
                error = fc2H264Open(aviContext, "SaveImageToAviEx_C-H264", &h264Option);
                if (error != FC2_ERROR_OK)
                {
                    printf("Error opening AVI: %d\n", error);
                }
                break;            
            }
        }

        error = fc2AVIAppend(aviContext, &rawImage);
        if (error != FC2_ERROR_OK)
        {
            printf("Error appending to AVI: %d\n", error);
        }
             
        printf("Appended image %d\n", i);        
    }  

    error = fc2DestroyImage(&rawImage);
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2CreateImaged: %d\n", error );           
        fc2DestroyAVI(aviContext);
        return -1;
    }

    error = fc2DestroyAVI(aviContext);
    if (error != FC2_ERROR_OK)
    {
        printf("Error in fc2DestroyAVI: %d\n", error);
        return -1;
    }

    return 0;
}



/*
 * Class:     WPointGreyApp
 * Method:    startVideo
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_startVideo
  (JNIEnv *env, jobject jobject, jint count, jstring fileName){
  
      fc2Error error;
    float frameRate = 0.0f;

    frameRate = GetFrameRate(pContext);
    if (frameRate == 0.0f)
    {
        printf("Invalid frame rate returned\n");
        return ;
    }

    SaveAVIHelper(pContext, H264, frameRate,count, "");

 }
	
	/*
 * Class:     WPointGreyApp
 * Method:    initContext
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_initContext
  (JNIEnv *en, jobject obj){
   printf("initializing context \n");
   fc2Error err = fc2CreateContext(&pContext );
   
   if ( err != FC2_ERROR_OK )
    {
        printf( "Error in fc2CreateContext: %s\n", fc2ErrorToDescription( err) );
        exit(-1) ;
    }  
   err=fc2GetCameraFromIndex(pContext,0,&pGuid );

    if ( err != FC2_ERROR_OK )
    {
        printf( "Error in fc2GetNumOfCameras: %s\n", fc2ErrorToDescription( err) );
        exit(-1) ;
    }

  }

  
  
  /*
 * Class:     WPointGreyApp
 * Method:    startCapture
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_startCapture
  (JNIEnv *env, jobject obj, jint count, jstring fileName){
   SetTimeStamping( pContext, TRUE );
   fc2Error error = fc2StartCapture( pContext );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
        return ;
    }     
	
	 const char *inCStr =  (*env)->GetStringUTFChars(env,fileName, 0);
     GrabImages( pContext, (int)count ,inCStr ); 
	(*env)->ReleaseStringUTFChars(env,fileName,inCStr);
	
  }


/*
 * Class:     WPointGreyApp
 * Method:    stopCapture
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_stopCapture
  (JNIEnv *env, jobject obj){
  
 fc2Error error = fc2StopCapture( pContext );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2StopCapture: %s\n", fc2ErrorToDescription( error) );
        return ;
    }
  }
  
  
  /*
 * Class:     WPointGreyApp
 * Method:    connect
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_connect
  (JNIEnv *env, jobject obj){
  fc2Error err = fc2Connect( pContext, &pGuid );
    if ( err != FC2_ERROR_OK )
    {
        printf( "Error in fc2Connect: %s\n", fc2ErrorToDescription( err) );
        exit(-1) ;
    }
  
  }
  
  /*
 * Class:     WPointGreyApp
 * Method:    disconnect
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_disconnect
  (JNIEnv *env, jobject obj){
  fc2Error error = fc2Disconnect( pContext );
	
	 if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2Disconnect: %s\n", fc2ErrorToDescription( error)
		);
        return ;
    }

  }
  
  /*
 * Class:     WPointGreyApp
 * Method:    destroysContext
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_destroysContext
  (JNIEnv *env, jobject obj){
   printf("destroying context \n");
   fc2Error error = fc2DestroyContext( pContext );  
   if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2DestroyContext: %s\n", fc2ErrorToDescription( error) );
        return ;
    }
  }
/*
 * Class:     WPointGreyApp
 * Method:    printCamera
 * Signature: ()I
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_printCamera
  (JNIEnv *env, jobject obj){
    fc2PropertyInfo propInfo ;
    fc2Error error;
    fc2CameraInfo camInfo;
    error = fc2GetCameraInfo( pContext, &camInfo );
    if ( error != FC2_ERROR_OK )
    {
	   printf( "Error in fc2GetCameraInfo: %s\n", fc2ErrorToDescription( error) );
        return ;
    }

    printf(
        "\n*** CAMERA INFORMATION ***\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
        camInfo.serialNumber,
        camInfo.modelName,
        camInfo.vendorName,
        camInfo.sensorInfo,
        camInfo.sensorResolution,
        camInfo.firmwareVersion,
        camInfo.firmwareBuildTime );
}
/*
 * Class:     WPointGreyApp
 * Method:    getNumCamera
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_WPointGreyApp_getNumCamera
  (JNIEnv *env, jobject obj) {
  
   /*fc2Error err;*/
   unsigned int pNumCameras = 0 ;
   fc2GetNumOfCameras(pContext,&pNumCameras);
   fc2DestroyContext( pContext );
   jint result = (jint)pNumCameras;
   return result ;
  
}

