//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#define CV_NO_BACKWARD_COMPATIBILITY
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#ifdef _WIN32
 #include <windows.h>
#else
 #include <unistd.h>
#endif

//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
IplImage *detect_and_draw(IplImage *img, CvMemStorage *storage, CvHaarClassifierCascade *cascade, void *RESULT, int width, int height, int put_roi, char *format, int biggest_object);
int findNearestNeighbor(float *projectedTestFace, float *pConfidence, int nTrainFaces, int nEigens, CvMat *projectedTrainFaceMat, CvMat *eigenValMat);
IplImage *resizeImage(const IplImage *origImg, int newWidth, int newHeight, bool keepAspectRatio);
void storeTrainingData(char *filename, int nEigens, int nTrainFaces, IplImage **eigenVectArr, CvMat *eigenValMat, CvMat *projectedTrainFaceMat, IplImage *pAvgTrainImg, int width, int height);
int loadTrainingData(char *filename, int *nEigens, int *nTrainFaces, IplImage ***eigenVectArr, CvMat **eigenValMat, CvMat **projectedTrainFaceMat, IplImage **pAvgTrainImg, int *width, int *height);

struct TrainingContainer {
    int      nTrainFaces;
    int      width;
    int      height;
    int      nEigens;
    IplImage **eigenVectArr;
    IplImage **faceImgArr;
    CvMat    *eigenValMat;
    CvMat    *projectedTrainFaceMat;
    IplImage *pAvgTrainImg;
};

TrainingContainer *CreateTrainingContainer(int nTrainFaces = 1, int width = 200, int height = 200, int nEigens = 0, IplImage **eigenVectArr = 0, IplImage **faceImgArr = 0, CvMat *eigenValMat = 0, CvMat *projectedTrainFaceMat = 0, IplImage *pAvgTrainImg = 0) {
    TrainingContainer *tc = (TrainingContainer *)cvAlloc(sizeof(TrainingContainer));

    tc->nTrainFaces           = nTrainFaces;
    tc->width                 = width;
    tc->height                = height;
    tc->nEigens               = nEigens;
    tc->eigenVectArr          = eigenVectArr;
    tc->faceImgArr            = faceImgArr;
    tc->eigenValMat           = eigenValMat;
    tc->projectedTrainFaceMat = projectedTrainFaceMat;
    tc->pAvgTrainImg          = pAvgTrainImg;
    return tc;
}

void DoneTrainingContainer(TrainingContainer *tc) {
    int i;

    if (!tc)
        return;

    if (tc->faceImgArr) {
        for (i = 0; i < tc->nTrainFaces; i++) {
            if (tc->faceImgArr[i])
                cvReleaseImage(&tc->faceImgArr[i]);
        }
        cvFree(&tc->faceImgArr);
    }
    if (tc->eigenVectArr) {
        for (i = 0; i < tc->nEigens; i++) {
            if (tc->eigenVectArr[i])
                cvReleaseImage(&tc->eigenVectArr[i]);
        }
        cvFree(&tc->eigenVectArr);
    }
    if (tc->eigenValMat)
        cvReleaseMat(&tc->eigenValMat);
    if (tc->pAvgTrainImg)
        cvReleaseImage(&tc->pAvgTrainImg);
    if (tc->projectedTrainFaceMat)
        cvReleaseMat(&tc->projectedTrainFaceMat);

    cvFree(&tc);
}

//-----------------------------------------------------//
#define OBJECT_FILE      0
#define OBJECT_CAMERA    1
#define OBJECT_BUFFER    2

CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    DEFINE_ECONSTANT(OBJECT_FILE)
    DEFINE_ECONSTANT(OBJECT_CAMERA)
    DEFINE_ECONSTANT(OBJECT_BUFFER)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(LoadObject, 1)
    T_STRING(LoadObject, 0)
    CvHaarClassifierCascade * cascade = (CvHaarClassifierCascade *)cvLoad(PARAM(0), 0, 0, 0);
    RETURN_NUMBER((SYS_INT)cascade);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CloseObject, 1)
    T_NUMBER(CloseObject, 0)
    CvHaarClassifierCascade * cascade = (CvHaarClassifierCascade *)PARAM_INT(0);
    if (cascade) {
        SET_NUMBER(0, 0)
        cvReleaseHaarClassifierCascade(&cascade);
    }
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ObjectDetect, 2, 8)
    T_STRING(ObjectDetect, 0)
    CHECK_STATIC_PARAM(1, "ObjectDetect: second parameter must be an object handle or a filename");
    char *cascade_name = 0;
    CvHaarClassifierCascade *cascade2 = 0;
    if (TYPE == VARIABLE_STRING) {
        T_STRING(ObjectDetect, 1)
        cascade_name = PARAM(1);
    } else
    if (TYPE == VARIABLE_NUMBER) {
        T_NUMBER(ObjectDetect, 1)
        cascade2 = (CvHaarClassifierCascade *)PARAM_INT(1);
        if (!cascade2) {
            RETURN_NUMBER(-2);
            return 0;
        }
    }
    int        width          = 40;
    int        height         = 40;
    int        type           = OBJECT_FILE;
    int        r_iterations   = 5;
    int        put_roi        = 0;
    int        biggest_object = 0;
    AnsiString fmt            = (char *)".";
    char       *format        = 0;

    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(ObjectDetect, 2)
        type = PARAM_INT(2);
    }

    if (PARAMETERS_COUNT > 4) {
        T_STRING(ObjectDetect, 4)
        if (PARAM_LEN(4)) {
            fmt    += PARAM(4);
            format  = fmt.c_str();
            put_roi = 1;
        }
    }

    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(ObjectDetect, 3)
        biggest_object = PARAM_INT(3);
    }

    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(ObjectDetect, 5)
        width = PARAM_INT(5);
        if (width < 2)
            width = 2;
        height = width;
    }

    if (PARAMETERS_COUNT > 6) {
        T_NUMBER(ObjectDetect, 6)
        height = PARAM_INT(6);
        if (height < 4)
            height = 4;
    }

    if (PARAMETERS_COUNT > 7) {
        T_NUMBER(ObjectDetect, 7)
        r_iterations = PARAM_INT(7);
        if (r_iterations < 0)
            r_iterations = 0;
    }

    CvMemStorage            *storage = 0;
    CvHaarClassifierCascade *cascade = 0;
    char      *input_name            = PARAM(0);
    CvCapture *capture = 0;
    IplImage  *frame, *frame_copy = 0;

    cascade = cascade2 ? cascade2 : (CvHaarClassifierCascade *)cvLoad(cascade_name, 0, 0, 0);

// Check whether the cascade has loaded successfully. Else report and error and quit
    if (!cascade) {
        //fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        RETURN_NUMBER(-2);
        return 0;
    }


    storage = cvCreateMemStorage(0);
// Find whether to detect the object from file or from camera.
//if( !input_name || (isdigit(input_name[0]) /*&& input_name[1] == '\0'*/) )
    if (type == 1) {
        if (PARAM_LEN(0)) {
            capture = cvCreateCameraCapture(input_name[0] - '0');
        } else
            capture = cvCreateCameraCapture(CV_CAP_ANY);
    }

    RETURN_NUMBER(-10);

    if (capture) {
        //cvSetCaptureProperty(capture, CV_CAP_PROP_MODE, MODE_640x480_MONO);
        // Capture from the camera.
        //for(;;) {
        // Capture the frame and load it in IplImage
        while (cvGrabFrame(capture)) {
            frame = cvRetrieveFrame(capture);

            // If the frame does not exist, quit the loop
            if (frame) {
                // Allocate framecopy as the same size of the frame
                if (!frame_copy)
                    frame_copy = cvCreateImage(cvSize(frame->width, frame->height),
                                               IPL_DEPTH_8U, /*frame->nChannels*/ 1);

                // Check the origin of image. If top left, copy the image frame to frame_copy.
                if (frame->origin == IPL_ORIGIN_TL)
                    //    cvCopy( frame, frame_copy, 0 );
                    cvConvertImage(frame, frame_copy, 0);
                // Else flip and copy the image
                else
                    cvConvertImage(frame, frame_copy, CV_CVTIMG_FLIP);
                //cvFlip( frame, frame_copy, 0 );

                InvokePtr(INVOKE_CREATE_ARRAY, RESULT);
                detect_and_draw(frame_copy, storage, cascade, RESULT, width, height, put_roi, format, biggest_object);
                //cvSaveImage ("out.png", frame_copy);
                //cvReleaseImage( &frame );
                cvReleaseImage(&frame_copy);
                // Call the function to detect and draw the face

                /*if (temp)
                    cvSaveImage ("out.png", temp);*/
                r_iterations--;
                if ((r_iterations <= 0) || (InvokePtr(INVOKE_GET_ARRAY_COUNT, RESULT)))
                    break;
            } else
                break;
        }
        cvReleaseCapture(&capture);
    } else
    if ((type == OBJECT_FILE) || (type == OBJECT_BUFFER)) {
        if (type == OBJECT_FILE)
            frame = cvLoadImage(input_name, CV_LOAD_IMAGE_GRAYSCALE);
        else {
            CvMat buf = cvMat(1, PARAM_LEN(0), CV_8UC1, PARAM(0));
            frame = cvDecodeImage(&buf, CV_LOAD_IMAGE_GRAYSCALE);
        }
        if (frame) {
            frame_copy = cvCreateImage(cvSize(frame->width, frame->height),
                                       IPL_DEPTH_8U, 1);

            cvEqualizeHist(frame, frame_copy);

            CREATE_ARRAY(RESULT);
            detect_and_draw(frame_copy, storage, cascade, RESULT, width, height, put_roi, format, biggest_object);
            //if (temp)
            //    cvSaveImage ("out.png", temp);
            cvReleaseImage(&frame);
            cvReleaseImage(&frame_copy);
            //cvReleaseImage( &frame );
            //RETURN_NUMBER(0);
        }
    }
    if (!cascade2)
        cvReleaseHaarClassifierCascade(&cascade);
    cvReleaseMemStorage(&storage);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CloseRecognize, 1)
    T_NUMBER(CloseRecognize, 0)
    TrainingContainer * tc = (TrainingContainer *)PARAM_INT(0);
    if (tc) {
        DoneTrainingContainer(tc);
        SET_NUMBER(0, 0)
    }
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(FaceRecognize, 2, 6)
    T_STRING(FaceRecognize, 0)
    T_ARRAY(FaceRecognize, 1)
    int type = OBJECT_FILE;
    IplImage *frame    = 0;
    IplImage *training = 0;
    char     *filename = 0;
    int      i;
    int      no_xml_data = 1;

    int               nTrainFaces            = 1;
    int               width                  = 200;
    int               height                 = 200;
    int               nEigens                = nTrainFaces - 1;
    IplImage          **eigenVectArr         = 0;
    IplImage          **faceImgArr           = 0;
    CvMat             *eigenValMat           = 0;
    CvMat             *projectedTrainFaceMat = 0;
    IplImage          *pAvgTrainImg          = 0;
    TrainingContainer *owner_tc              = 0;

    nTrainFaces = InvokePtr(INVOKE_GET_ARRAY_COUNT, PARAMETER(1));
    RETURN_NUMBER(-1)

    if (PARAMETERS_COUNT > 5) {
        T_NUMBER(FaceRecognize, 5)
        owner_tc = (TrainingContainer *)PARAM_INT(5);
        if (owner_tc) {
            if (nTrainFaces != owner_tc->nTrainFaces) {
                // something changed ?
                // invalidate context
                DoneTrainingContainer(owner_tc);
                owner_tc = 0;
            } else {
                nTrainFaces           = owner_tc->nTrainFaces;
                width                 = owner_tc->width;
                height                = owner_tc->height;
                nEigens               = owner_tc->nEigens;
                eigenVectArr          = owner_tc->eigenVectArr;
                faceImgArr            = owner_tc->faceImgArr;
                eigenValMat           = owner_tc->eigenValMat;
                projectedTrainFaceMat = owner_tc->projectedTrainFaceMat;
                pAvgTrainImg          = owner_tc->pAvgTrainImg;
                no_xml_data           = 0;
            }
        }
    }

    if (PARAMETERS_COUNT > 2) {
        SET_NUMBER(2, 0);
    }

    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(FaceRecognize, 3)
        type = PARAM_INT(3);
    }

    if (PARAMETERS_COUNT > 4) {
        T_STRING(FaceRecognize, 4)
        if (PARAM_LEN(4)) {
            filename = PARAM(4);
        }
    }

    if ((!owner_tc) && (!nTrainFaces) && (filename)) {
        if (loadTrainingData(filename, &nEigens, &nTrainFaces, &eigenVectArr, &eigenValMat, &projectedTrainFaceMat, &pAvgTrainImg, &width, &height))
            no_xml_data = 0;
        else {
            width  = 200;
            height = 200;
        }
        filename = 0;
    }

    nEigens = nTrainFaces - 1;
    if (nTrainFaces >= 2) {
        if (type == OBJECT_FILE)
            frame = cvLoadImage(PARAM(0), CV_LOAD_IMAGE_GRAYSCALE);
        else {
            CvMat buf = cvMat(1, PARAM_LEN(0), CV_8UC1, PARAM(0));
            frame = cvDecodeImage(&buf, CV_LOAD_IMAGE_GRAYSCALE);
        }

        if (frame) {
            IplImage *frame2 = resizeImage(frame, width, height, true);
            if (frame2) {
                cvReleaseImage(&frame);
                frame  = frame2;
                frame2 = 0;
            }
            if (no_xml_data) {
                faceImgArr = (IplImage **)cvAlloc(sizeof(IplImage *) * nTrainFaces);
                for (i = 0; i < nTrainFaces; i++) {
                    void   *newpData = 0;
                    char   *szData   = 0;
                    NUMBER nData     = 0;

                    Invoke(INVOKE_ARRAY_VARIABLE, PARAMETER(1), (INTEGER)i, &newpData);
                    Invoke(INVOKE_GET_VARIABLE, newpData, &TYPE, &szData, &nData);

                    IplImage *img = 0;
                    if (TYPE == VARIABLE_STRING) {
                        if (type == OBJECT_FILE) {
                            faceImgArr[i] = cvLoadImage(szData, CV_LOAD_IMAGE_GRAYSCALE);
                        } else {
                            CvMat buf = cvMat(1, (int)nData, CV_8UC1, szData);
                            faceImgArr[i] = cvDecodeImage(&buf, CV_LOAD_IMAGE_GRAYSCALE);
                        }
                        if (faceImgArr[i]) {
                            img = resizeImage(faceImgArr[i], width, height, true);
                            cvReleaseImage(&faceImgArr[i]);
                        }
                    }
                    if (img) {
                        faceImgArr[i] = img;
                    } else {
                        for (int j = 0; j < i; j++) {
                            if (faceImgArr[j])
                                cvReleaseImage(&faceImgArr[i]);
                        }
                        cvFree(&faceImgArr);
                        cvReleaseImage(&frame);
                        RETURN_NUMBER(-2);
                        return 0;
                    }
                }

                // ==== PCA ==== //
                CvTermCriteria calcLimit;
                CvSize         faceImgSize;

                // set the number of eigenvalues to use

                // allocate the eigenvector images
                faceImgSize.width  = faceImgArr[0]->width;
                faceImgSize.height = faceImgArr[0]->height;
                eigenVectArr       = (IplImage **)cvAlloc(sizeof(IplImage *) * nEigens);
                for (i = 0; i < nEigens; i++)
                    eigenVectArr[i] = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

                // allocate the eigenvalue array
                eigenValMat = cvCreateMat(1, nEigens, CV_32FC1);

                // allocate the averaged image
                pAvgTrainImg = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

                // set the PCA termination criterion
                calcLimit = cvTermCriteria(CV_TERMCRIT_ITER, nEigens, 1);

                // compute average image, eigenvalues, and eigenvectors
                cvCalcEigenObjects(
                    nTrainFaces,
                    (void *)faceImgArr,
                    (void *)eigenVectArr,
                    CV_EIGOBJ_NO_CALLBACK,
                    0,
                    0,
                    &calcLimit,
                    pAvgTrainImg,
                    eigenValMat->data.fl);

                cvNormalize(eigenValMat, eigenValMat, 1, 0, CV_L1, 0);
                // ==== end of PCA ====//

                projectedTrainFaceMat = cvCreateMat(nTrainFaces, nEigens, CV_32FC1);
                int offset = projectedTrainFaceMat->step / sizeof(float);

                for (i = 0; i < nTrainFaces; i++) {
                    //int offset = i * nEigens;
                    cvEigenDecomposite(
                        faceImgArr[i],
                        nEigens,
                        eigenVectArr,
                        0, 0,
                        pAvgTrainImg,
                        //projectedTrainFaceMat->data.fl + i*nEigens);
                        projectedTrainFaceMat->data.fl + i * offset);
                }
                // end of learning //
                if (filename)
                    storeTrainingData(filename, nEigens, nTrainFaces, eigenVectArr, eigenValMat, projectedTrainFaceMat, pAvgTrainImg, width, height);
            }
            int success = 0;
            if (nEigens > 0) {
                float *projectedTestFace = (float *)cvAlloc(nEigens * sizeof(float));
                // project the test image onto the PCA subspace
                cvEigenDecomposite(
                    frame,
                    nEigens,
                    eigenVectArr,
                    0, 0,
                    pAvgTrainImg,
                    projectedTestFace);

                // Check which person it is most likely to be.
                float confidence = 0;
                int   iNearest   = findNearestNeighbor(projectedTestFace, &confidence, nTrainFaces, nEigens, projectedTrainFaceMat, eigenValMat);
                //int nearest  = trainPersonNumMat->data.i[iNearest];
                //printf("Most likely person in camera: '%i' (confidence=%f.\n", iNearest, confidence);
                RETURN_NUMBER(iNearest);
                if (PARAMETERS_COUNT > 2) {
                    SET_NUMBER(2, confidence);
                }
                success = 1;
                cvFree(&projectedTestFace);
            }
            // cleaning //
            // ====================//
            if ((success) && (PARAMETERS_COUNT > 5)) {
                if (!owner_tc) {
                    TrainingContainer *tc = CreateTrainingContainer(nTrainFaces, width, height, nEigens, eigenVectArr, faceImgArr, eigenValMat, projectedTrainFaceMat, pAvgTrainImg);
                    SET_NUMBER(5, (SYS_INT)tc);
                }
            } else
            if (!owner_tc) {
                if (faceImgArr) {
                    for (i = 0; i < nTrainFaces; i++) {
                        if (faceImgArr[i])
                            cvReleaseImage(&faceImgArr[i]);
                    }
                    cvFree(&faceImgArr);
                }
                if (eigenVectArr) {
                    for (i = 0; i < nEigens; i++) {
                        if (eigenVectArr[i])
                            cvReleaseImage(&eigenVectArr[i]);
                    }
                    cvFree(&eigenVectArr);
                }
                if (eigenValMat)
                    cvReleaseMat(&eigenValMat);
                if (pAvgTrainImg)
                    cvReleaseImage(&pAvgTrainImg);
                if (projectedTrainFaceMat)
                    cvReleaseMat(&projectedTrainFaceMat);
            }
            if (frame)
                cvReleaseImage(&frame);
            // ====================//
        }
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(ReadCamera, 0, 3)
    int camera = CV_CAP_ANY;
    AnsiString format = (char *)".png";
    int        delay  = 0;
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(ReadCamera, 0)
        camera = PARAM_INT(0);
    }
    if (PARAMETERS_COUNT > 1) {
        T_STRING(ReadCamera, 1)
        format  = (char *)".";
        format += PARAM(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(ReadCamera, 2)
        delay = PARAM_INT(2);
    }
    CvCapture *capture = cvCaptureFromCAM(camera);
    if (capture) {
        if (delay) {
#ifdef _WIN32
            Sleep(delay);
#else
            usleep(delay * 1000);
#endif
        }
        if (cvGrabFrame(capture)) {
            IplImage *frame = cvRetrieveFrame(capture);
            if (frame) {
                static int jpeg_params[] = { CV_IMWRITE_JPEG_QUALITY, 99, 0 };
                CvMat      *buf          = cvEncodeImage(format.c_str(), frame, jpeg_params);
                if (buf) {
                    int size = buf->rows * buf->cols;
                    RETURN_BUFFER((char *)buf->data.ptr, size);
                    cvReleaseMat(&buf);
                }
                //cvReleaseImage( &frame );
            }
        }
        cvReleaseCapture(&capture);
    } else
        RETURN_STRING("");
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(OpenCamera, 0, 3)
    int camera = CV_CAP_ANY;
    if (PARAMETERS_COUNT > 0) {
        T_NUMBER(OpenCamera, 0)
        camera = PARAM_INT(0);
    }
    CvCapture *capture = cvCaptureFromCAM(camera);
    if (capture) {
        if (PARAMETERS_COUNT > 1) {
            T_NUMBER(OpenCamera, 1)
            cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, PARAM(1));
        }
        if (PARAMETERS_COUNT > 2) {
            T_NUMBER(OpenCamera, 2)
            cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, PARAM(2));
        }
    }
    RETURN_NUMBER((SYS_INT)capture);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CloseCamera, 1)
    T_NUMBER(CloseCamera, 0)

    CvCapture * capture = (CvCapture *)PARAM_INT(0);
    if (capture) {
        SET_NUMBER(0, 0)
        cvReleaseCapture(&capture);
    }
    RETURN_NUMBER(0);
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(CameraFrame, 1, 2)
    T_HANDLE(CameraFrame, 0)
    AnsiString format = (char *)".png";
    if (PARAMETERS_COUNT > 1) {
        T_STRING(CameraFrame, 1)
        format  = (char *)".";
        format += PARAM(1);
    }

    CvCapture *capture = (CvCapture *)PARAM_INT(0);
    RETURN_STRING("");
    if (capture) {
        IplImage *frame = cvQueryFrame(capture);
        if (frame) {
            static int jpeg_params[] = { CV_IMWRITE_JPEG_QUALITY, 99, 0 };
            CvMat      *buf          = cvEncodeImage(format.c_str(), frame, jpeg_params);
            if (buf) {
                int size = buf->rows * buf->cols;
                RETURN_BUFFER((char *)buf->data.ptr, size);
                cvReleaseMat(&buf);
            }
        }
    }
END_IMPL
//-----------------------------------------------------//
// Function to detect and draw any faces that is present in an image
IplImage *detect_and_draw(IplImage *img, CvMemStorage *storage, CvHaarClassifierCascade *cascade, void *RESULT, int w, int h, int put_roi, char *format, int biggest_object) {
    static int jpeg_params[] = { CV_IMWRITE_JPEG_QUALITY, 99, 0 };

    if (!format)
        format = ".png";
    int scale = 1;
    // Create two points to represent the face locations
    CvPoint pt1, pt2;
    int     i;

    // Clear the memory storage which was used before
    cvClearMemStorage(storage);

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if (cascade) {
        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        CvSeq *faces = cvHaarDetectObjects(img, cascade, storage,
                                           1.1, 2, biggest_object ? CV_HAAR_FIND_BIGGEST_OBJECT : 0,
                                           cvSize(w, h));

        // Loop the number of faces found.
        for (i = 0; i < (faces ? faces->total : 0); i++) {
            // Create a new rectangle for drawing the face
            CvRect *r = (CvRect *)cvGetSeqElem(faces, i);

            // Find the dimensions of the face,and scale it if necessary
            pt1.x = r->x * scale;
            pt2.x = (r->x + r->width) * scale;
            pt1.y = r->y * scale;
            pt2.y = (r->y + r->height) * scale;

            // Draw the rectangle in the input image
            //cvRectangle( img, pt1, pt2, CV_RGB(255,255,255), 1, 8, 0 );

            void *ARR = 0;
            InvokePtr(INVOKE_ARRAY_VARIABLE, RESULT, (INTEGER)i, &ARR);
            if (ARR) {
                InvokePtr(INVOKE_CREATE_ARRAY, ARR);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"x", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)r->x);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"y", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)r->y);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"w", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)r->width);
                InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"h", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)r->height);
                if (put_roi) {
                    cvSetImageROI(img, *r);
                    IplImage *face = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
                    if (face) {
                        cvCopy(img, face, NULL);
                        CvMat *buf = cvEncodeImage(format, face, jpeg_params);
                        if (buf) {
                            int size = buf->rows * buf->cols;
                            InvokePtr(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, ARR, (char *)"data", (INTEGER)VARIABLE_STRING, (char *)buf->data.ptr, (NUMBER)size);
                            cvReleaseMat(&buf);
                        }
                        cvReleaseImage(&face);
                    }
                    cvResetImageROI(img);
                }
            }
        }
    }

    return img;
}

//-----------------------------------------------------//
#define USE_MAHALANOBIS_DISTANCE
int findNearestNeighbor(float *projectedTestFace, float *pConfidence, int nTrainFaces, int nEigens, CvMat *projectedTrainFaceMat, CvMat *eigenValMat) {
    //double leastDistSq = 1e12;
    double leastDistSq = DBL_MAX;
    int    i, iTrain, iNearest = 0;

    for (iTrain = 0; iTrain < nTrainFaces; iTrain++) {
        double distSq = 0;

        for (i = 0; i < nEigens; i++) {
            float d_i = projectedTestFace[i] - projectedTrainFaceMat->data.fl[iTrain * nEigens + i];
#ifdef USE_MAHALANOBIS_DISTANCE
            distSq += d_i * d_i / eigenValMat->data.fl[i]; // Mahalanobis distance (might give better results than Eucalidean distance)
#else
            distSq += d_i * d_i;                           // Euclidean distance.
#endif
        }

        if (distSq < leastDistSq) {
            leastDistSq = distSq;
            iNearest    = iTrain;
        }
    }

    // Return the confidence level based on the Euclidean distance,
    // so that similar images should give a confidence between 0.5 to 1.0,
    // and very different images should give a confidence between 0.0 to 0.5.
    *pConfidence = 1.0f - sqrt(leastDistSq / (float)(nTrainFaces * nEigens)) / 255.0f;

    // Return the found index.
    return iNearest;
}

//-----------------------------------------------------//
IplImage *cropImage(const IplImage *img, const CvRect region) {
    IplImage *imageCropped;
    CvSize   size;

    if ((img->width <= 0) || (img->height <= 0) ||
        (region.width <= 0) || (region.height <= 0)) {
        //cerr << "ERROR in cropImage(): invalid dimensions." << endl;
        exit(1);
    }

    if (img->depth != IPL_DEPTH_8U) {
        //cerr << "ERROR in cropImage(): image depth is not 8." << endl;
        exit(1);
    }

    // Set the desired region of interest.
    cvSetImageROI((IplImage *)img, region);
    // Copy region of interest into a new iplImage and return it.
    size.width   = region.width;
    size.height  = region.height;
    imageCropped = cvCreateImage(size, IPL_DEPTH_8U, img->nChannels);
    cvCopy(img, imageCropped);          // Copy just the region.

    return imageCropped;
}

//-----------------------------------------------------//
void storeTrainingData(char *filename, int nEigens, int nTrainFaces, IplImage **eigenVectArr, CvMat *eigenValMat, CvMat *projectedTrainFaceMat, IplImage *pAvgTrainImg, int width, int height) {
    CvFileStorage *fileStorage;
    int           i;

    // create a file-storage interface
    fileStorage = cvOpenFileStorage(filename, 0, CV_STORAGE_WRITE);
    // store all the data
    cvWriteInt(fileStorage, "nEigens", nEigens);
    cvWriteInt(fileStorage, "width", width);
    cvWriteInt(fileStorage, "height", height);
    cvWriteInt(fileStorage, "nTrainFaces", nTrainFaces);
    //cvWrite(fileStorage, "trainPersonNumMat", personNumTruthMat, cvAttrList(0,0));
    cvWrite(fileStorage, "eigenValMat", eigenValMat, cvAttrList(0, 0));
    cvWrite(fileStorage, "projectedTrainFaceMat", projectedTrainFaceMat, cvAttrList(0, 0));
    cvWrite(fileStorage, "avgTrainImg", pAvgTrainImg, cvAttrList(0, 0));
    for (i = 0; i < nEigens; i++) {
        char varname[200];
        sprintf(varname, "eigenVect_%d", i);
        cvWrite(fileStorage, varname, eigenVectArr[i], cvAttrList(0, 0));
    }

    // release the file-storage interface
    cvReleaseFileStorage(&fileStorage);
}

//-----------------------------------------------------//
int loadTrainingData(char *filename, int *nEigens, int *nTrainFaces, IplImage ***eigenVectArr, CvMat **eigenValMat, CvMat **projectedTrainFaceMat, IplImage **pAvgTrainImg, int *width, int *height) {
    CvFileStorage *fileStorage;

    *nEigens               = 0;
    *nTrainFaces           = 0;
    *eigenVectArr          = 0;
    *eigenValMat           = 0;
    *projectedTrainFaceMat = 0;
    *pAvgTrainImg          = 0;
    *width  = 0;
    *height = 0;
    int i;

    // create a file-storage interface
    fileStorage = cvOpenFileStorage(filename, 0, CV_STORAGE_READ);
    if (!fileStorage) {
        return 0;
    }

    //nPersons = cvReadIntByName( fileStorage, 0, "nPersons", 0 );
    //if (nPersons == 0) {
    //	return 0;
    //}

    // Load the data
    *nEigens               = cvReadIntByName(fileStorage, 0, "nEigens", 0);
    *width                 = cvReadIntByName(fileStorage, 0, "width", 200);
    *height                = cvReadIntByName(fileStorage, 0, "height", 200);
    *nTrainFaces           = cvReadIntByName(fileStorage, 0, "nTrainFaces", 0);
    *eigenValMat           = (CvMat *)cvReadByName(fileStorage, 0, "eigenValMat", 0);
    *projectedTrainFaceMat = (CvMat *)cvReadByName(fileStorage, 0, "projectedTrainFaceMat", 0);
    *pAvgTrainImg          = (IplImage *)cvReadByName(fileStorage, 0, "avgTrainImg", 0);
    *eigenVectArr          = (IplImage **)cvAlloc(*nTrainFaces * sizeof(IplImage *));
    for (i = 0; i < *nEigens; i++) {
        char varname[200];
        sprintf(varname, "eigenVect_%d", i);
        (*eigenVectArr)[i] = (IplImage *)cvReadByName(fileStorage, 0, varname, 0);
    }

    cvReleaseFileStorage(&fileStorage);
    return 1;
}

//-----------------------------------------------------//
IplImage *resizeImage(const IplImage *origImg, int newWidth, int newHeight, bool keepAspectRatio) {
    IplImage *outImg = 0;
    int      origWidth;
    int      origHeight;

    if (origImg) {
        origWidth  = origImg->width;
        origHeight = origImg->height;
    }
    if ((newWidth <= 0) || (newHeight <= 0) || (origImg == 0) ||
        (origWidth <= 0) || (origHeight <= 0)) {
        return 0;
    }

    if (keepAspectRatio) {
        // Resize the image without changing its aspect ratio,
        // by cropping off the edges and enlarging the middle section.
        CvRect r;
        // input aspect ratio
        float origAspect = (origWidth / (float)origHeight);
        // output aspect ratio
        float newAspect = (newWidth / (float)newHeight);
        // crop width to be origHeight * newAspect
        if (origAspect > newAspect) {
            int tw = (origHeight * newWidth) / newHeight;
            r = cvRect((origWidth - tw) / 2, 0, tw, origHeight);
        } else {        // crop height to be origWidth / newAspect
            int th = (origWidth * newHeight) / newWidth;
            r = cvRect(0, (origHeight - th) / 2, origWidth, th);
        }
        IplImage *croppedImg = cropImage(origImg, r);

        // Call this function again, with the new aspect ratio image.
        // Will do a scaled image resize with the correct aspect ratio.
        outImg = resizeImage(croppedImg, newWidth, newHeight, false);
        cvReleaseImage(&croppedImg);
    } else {
        // Scale the image to the new dimensions,
        // even if the aspect ratio will be changed.
        outImg = cvCreateImage(cvSize(newWidth, newHeight),
                               origImg->depth, origImg->nChannels);
        if ((newWidth > origImg->width) && (newHeight > origImg->height)) {
            // Make the image larger
            cvResetImageROI((IplImage *)origImg);
            // CV_INTER_LINEAR: good at enlarging.
            // CV_INTER_CUBIC: good at enlarging.
            cvResize(origImg, outImg, CV_INTER_LINEAR);
        } else {
            // Make the image smaller
            cvResetImageROI((IplImage *)origImg);
            // CV_INTER_AREA: good at shrinking (decimation) only.
            cvResize(origImg, outImg, CV_INTER_AREA);
        }
    }
    return outImg;
}

