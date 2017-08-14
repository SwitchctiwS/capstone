from __future__ import print_function
from imutils.object_detection import non_max_suppression
from imutils import paths
import numpy as np
import argparse
import imutils
import cv2

cap = cv2.VideoCapture(0)
fourcc = cv2.VideoWriter_fourcc(*"XVID")
output = cv2.VideoWriter("yee.avi", fourcc, 15.0, (640,480))

# Set up tracker.
# Instead of MIL, you can also use
# BOOSTING, KCF, TLD, MEDIANFLOW or GOTURN
#tracker = cv2.Tracker_create("MIL")

#bbox = (287, 23, 86, 320)
#ret = tracker.init(frame, bbox)

hog = cv2.HOGDescriptor()
hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

while True:
    ret, frame = cap.read()
    if ret is False:
        break
	
    """
    frame = imutils.resize(frame, width=min(400, frame.shape[1]))
    
    (rects, weights) = hog.detectMultiScale(frame, winStride=(4, 4), padding=(8, 8), scale=1.05)

    for (x, y, w, h) in rects:
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

    rects = np.array([[x, y, x + w, y + h] for (x, y, w, h) in rects])
    pick = non_max_suppression(rects, probs=None, overlapThresh=0.65)

    for (xA, yA, xB, yB) in pick:
        cv2.rectangle(frame, (xA, yA), (xB, yB), (0, 255, 0), 2)
    """

    # Exit if 'q' pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    
    cv2.imshow("frame", frame)
    output.write(frame)
    
cap.release()
cv2.destroyAllWindows()
