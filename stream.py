import cv2

cap = cv2.VideoCapture(0)


cnt = 0

while True:
    ret, img = cap.read()

    cnt += 1
    if cnt % 5 != 0:
        continue
    
    img = cv2.resize(img, (680, 460))
    
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    low_ = np.array([0, 0, 0])
    max_ = np.array([0, 0, 0])
    frame = cv2.inRange(hsv, low_, max_)

    cv2.imshow('stream', frame)