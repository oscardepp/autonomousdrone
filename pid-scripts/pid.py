import jetson_inference
import jetson_utils
import numpy as np

net = jetson_inference.detectNet(model="/home/drone/autonomous-drone/models/box_custom/ssd-mobilenet.onnx", labels="/home/drone/autonomous-drone/models/box_custom_basic/labels.txt", input_blob="input_0", output_cvg="scores", output_bbox="boxes", threshold=0.5)
camera = jetson_utils.videoSource("/dev/video0")
display = jetson_utils.videoOutput()

'''
PID Loop:
3 Constants: kp - Proportional, ki - Integral, kd - Derivative
'''
kp, ki, kd = 1, 1, 1
total, prior = 0, 0
desired = np.array([640, 360])
while True:
	image = camera.Capture()
	detections = np.array(net.Detect(image))
#	Loop Solution:
# 	centerlist = np.zeros((detections.shape[0], 2))
# 	for i in range(centerlist.shape[0]):
# 		centerlist[i, :] = np.array(Detections[i].center)
	func = np.vectorize(lambda x : x.center)
	centerlist = np.apply_along_axis(func, 0, Detections).transpose()
	current = np.average(centerlist, axis=0)
	error = desired - current

	proportional = kp*error
	
	total += error
	integral = ki*total

	derivative = kd*(error - prior)
	prior = error
	pidresult = proportional + derivative + integral
	print(pidresult)

	display.Render(image)
	display.SetStatus("Object Detection | Network {:.0f} FPS".format(net.GetNetworkFPS()))
