import jetson_inference
import jetson_utils

net = jetson_inference.detectNet("ssd-mobilenet-v2", threshold=0.5)
camera = jetson_utils.videoSource("/dev/video0")
display = jetson_utils.videoOutput()

while True:
	image = camera.Capture()
	detections = net.Detect(image)
	display.Render(image)
	display.SetStatus("Object Detection | Network {:.0f} FPS".format(net.GetNetworkFPS()))

