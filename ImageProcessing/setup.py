import cv2
import pickle
import numpy as np
import tkinter as tk  # implement this later

filename = '/Users/Luke/Desktop/Python_Stuff/ParkingSpotDetection/screenshot.png'
points = []

def mark_points(img):
    global points
    spots = []
    # Mouse callback function

    def mouse_callback(event, x, y, flags, param):
        global points

        if event == cv2.EVENT_RBUTTONDOWN:
            points.append([x, y])
            if len(points) % 4 == 0 and len(points) >= 4:
                pts = [points[-4], points[-3], points[-2], points[-1]]
                pts = np.array(pts, np.int32).reshape((-1, 1, 2))
                spots.append(pts)
                cv2.polylines(img, [pts], True, (0, 255, 0), 10)
                cv2.imshow("Image", img)

    # Create a window and set the mouse callback function
    cv2.namedWindow("Image")
    cv2.setMouseCallback("Image", mouse_callback)

    # Display the image
    cv2.imshow("Image", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    return spots


def draw_polygons(polygons, img):
    # Draw polygons using the defined points
    for polygon in polygons:
        pts = np.array(polygon, np.int32)
        pts = pts.reshape((-1, 1, 2))
        cv2.polylines(img, [pts], True, (0, 255, 0), 10)

    # Display the image
    cv2.imshow("Image", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


def set_spots(img):
    poly = mark_points(img)
    with open('ParkingLotPos', 'wb') as file:
        pickle.dump(poly, file)


def show_spots(img):
    try:
        with open('ParkingLotPos', 'rb') as file:
            poly = pickle.load(file)
    except FileNotFoundError:
        print_error("No spots set yet. Please set the spots first.")
        return  # Exit the function if spots file not found

    if not poly:
        print_error("No spots set yet. Please set the spots first.")
        return  # Exit the function if spots list is empty

    draw_polygons(poly, img)

def print_error(message):
    bar = "\n" + ((len(message) + 16)  * "=") + "\n"
    error_str = bar + "ERROR:\n\t" + \
        message + "\n" + bar
    print("\033[91m{}\033[00m" .format(error_str))

def print_success(message):
    bar = "\n" + ((len(message) + 16)  * "=") + "\n"
    success_str = bar + "SUCCESS:\n\t" + \
        message + "\n" + bar
    print("\033[92m{}\033[00m" .format(success_str))

def print_welcome():
    message = """Welcome!
                Here are some instructions to get you started:"""
    bar = "\n" + ((len(message) + 16)  * "=") + "\n"
    welcome_str = bar + "SUCCESS:\n\t" + \
        message + "\n" + bar
    print("\033[94m{}\033[00m" .format(welcome_str))


def setup(img):
    print_welcome()
    while True:
        try:
            option = int(input(
                "Please select an option for the setup:\n\n(1): Set up spots\n(2): Display spots\n(3): Exit\n\nChoice: "))
            print('Press `ESC` to terminate the program once you have selected the spots or are done viewing the spots.')
            if option == 1:
                set_spots(img)
            elif option == 2:
                show_spots(img)
            elif option == 3:
                print_success("Exiting program successfully.\n\nGOODBYE")
                return 1
            else:
                raise ValueError("Please enter a valid INTEGER (1, 2, or 3).")
        except ValueError as e:
            print_error(str(e))

if __name__ == "__main__":
    img = cv2.imread(filename)
    setup(img)
