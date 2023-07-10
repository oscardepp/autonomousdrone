import jetson_inference
import jetson_utils

net = jetson_inference.detectNet(model="/home/drone/autonomous-drone/models/box_custom/ssd-mobilenet.onnx", labels="/home/drone/autonomous-drone/models/box_custom_basic/labels.txt", input_blob="input_0", output_cvg="scores", output_bbox="boxes", threshold=0.5)
camera = jetson_utils.videoSource("/dev/video0")
display = jetson_utils.videoOutput()

while True:
	image = camera.Capture()
	detections = net.Detect(image)
	display.Render(image)
	display.SetStatus("Object Detection | Network {:.0f} FPS".format(net.GetNetworkFPS()))
