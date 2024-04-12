import cv2
import pickle
import cvzone
import numpy as np
import time
import climage
from setup import *


def check_parking_space(img, proc_img, poly):
    spaces = 0
    for polygon in poly:
        pts = np.array(polygon, np.int32)
        pts = pts.reshape((-1, 1, 2))

        # Find the bounding rectangle around the polygon
        x, y, w, h = cv2.boundingRect(pts)

        # Crop the original image using the bounding box
        cropped_img = proc_img[y:y+h, x:x+w]

        # Create an empty mask
        mask = np.zeros_like(cropped_img)

        # Fill the polygon with white color (255) on the mask
        cv2.fillPoly(mask, [pts - (x, y)], (255, 255, 255))

        # Apply the mask to the cropped image using bitwise AND operation
        cropped_img = cv2.bitwise_and(cropped_img, mask)

        count = cv2.countNonZero(cropped_img)
        cvzone.putTextRect(img, str(count), (x + int(w/2.5),
                           y + h - 5), scale=2, thickness=1, offset=2)

        if count < 1000:
            color = (0, 255, 0)
            thickness = 5
            spaces += 1
        else:
            color = (0, 0, 255)
            thickness = 2
        cv2.polylines(img, [pts], True, color, thickness)
    counter = str(spaces) + "/" + str(len(poly))
    h, w = img.shape[:2]
    cvzone.putTextRect(img, counter, (w//2, 50), scale=3)

    return counter


def process_image(img):
    imgGray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    imgBlur = cv2.GaussianBlur(imgGray, (3, 3), 1)
    imgThreshold = cv2.adaptiveThreshold(
        imgBlur, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 25, 16)
    imgMedian = cv2.medianBlur(imgThreshold, 7)
    kernel = np.ones((3, 3), np.int8)
    imgDilate = cv2.dilate(imgMedian, kernel, iterations=2)
    return imgDilate


def main():
    cap = cv2.VideoCapture(
        'ImageProcessing/parking_lot_video.mp4')
    # getting the spot data
    with open('ImageProcessing/ParkingLotPos', 'rb') as file:
        poly = pickle.load(file)

    timer = time.time()

    while time.time() - timer < 5:
        # if current frame == last frame
        if cap.get(cv2.CAP_PROP_POS_FRAMES) == cap.get(cv2.CAP_PROP_FRAME_COUNT):
            cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
        success, img = cap.read()

        img_proc = process_image(img)

        counter = check_parking_space(img, img_proc, poly)

        cv2.imshow("Image", img)

        cv2.waitKey(1)

    return counter


if __name__ == '__main__':
    counter = main()
    print(counter)
