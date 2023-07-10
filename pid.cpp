#include "detectNet.h"
#include "videoSource.h"
#include "videoOutput.h"

int main(int argc, char**argv){
    detectNet* net = detectNet::Create("/home/drone/autonomous-drone/models/box_custom/ssd-mobilenet.onnx", "/home/drone/autonomous-drone/models/box_custom_basic/labels.txt", "input_0", "scores", "boxes", 0.5f);
    videoSource* camera = videoSource::Create("/dev/video0");
    videoOutput* output = videoOutput::Create();
    

    //PID Constants
    float kp = 1f;
    float ki = 1f;
    float kd = 1f;

    float total[2] = {0f, 0f};
    float prior[2] = {0f, 0f};
    float desired[2] = {640f, 360f}

    while(true){
        uchar3* image = NULL;

        if(!camera->Capture(&image)){
            if(!camera->IsStreaming()){
                break;
            }

            LogError("detectnet: failed to capture video frame\n");
            continue;
        }

        detectNet::Detection* detections = NULL;
        
        const int detectionsLen = net->Detect(image, &detections);

        //Check if there are no detections
        if(detectionslen <= 0){
            continue;
        }

        //Get center coordinates for each detection in detections
        for(int i = 0; i < detectionsLen; i++){
            //Get center x coordinate of detection
            float leftCoord = detections[i].Left;
            float xOffset = detections[i].Width() / 2f;
            float xCenter = leftCoord + xOffset;

            //Get center y coordinate of detection
            float topCoord = detections[i].Top;
            float yOffset = detections[i].Height() / 2f;
            float yCenter = topCoord + yOffset;

            detections[i] = {xCenter, yCenter};
        }

        //Calculate average center coordinates
        float current[2] = {0f; 0f};
        //Sum up all center coordinates
        for(int i = 0; i < detectionsLen; i++){
            current = {current[0] + detections[i][0], current[1] + detections[i][1]};
        }
        current = {current[0]/((float) detectionsLen), current[1]/((float) detectionsLen)};
       
        //Calculate error
        float error[2] = {desired[0]- current[0], desired[1] - current[1]};

        //Calculate proportional
        float proportional[2] = {kp * error[0], kp * error[1]};

        //Update total
        total = {total[0] + error[0], total[1] + error[1]}; 

        //Calculate integral
        float integral[2] = {ki * total[0], ki * total[1]};

        //Calculate derivative
        float derivative[2] = {kd * (error[0] - prior[0]), kd * (error[1] - prior[1])};
        
        //Update prior
        prior = error;

        pidResult = {proportional[0] + derivative[0] + integral[1], proportional[1] + derivative[1] + integral[1]}
        
        if(output != null){
            output->Render(image);

            char str[256];
			sprintf(str, "Object Detection | Network %.0f FPS", net->GetNetworkFPS());
            output->SetStatus(str);
        }
    }
}