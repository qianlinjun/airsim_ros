;; Auto-generated. Do not edit!


(when (boundp 'airsim_bridge::ImageMosaicer)
  (if (not (find-package "AIRSIM_BRIDGE"))
    (make-package "AIRSIM_BRIDGE"))
  (shadow 'ImageMosaicer (find-package "AIRSIM_BRIDGE")))
(unless (find-package "AIRSIM_BRIDGE::IMAGEMOSAICER")
  (make-package "AIRSIM_BRIDGE::IMAGEMOSAICER"))

(in-package "ROS")
;;//! \htmlinclude ImageMosaicer.msg.html
(if (not (find-package "SENSOR_MSGS"))
  (ros::roseus-add-msgs "sensor_msgs"))
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass airsim_bridge::ImageMosaicer
  :super ros::object
  :slots (_header _id _uav _grafo _edges _tx _ty _theta _scale _theta_offset _latitude _longitude _altitude _heading _pitch _roll _camera_type _filename _image ))

(defmethod airsim_bridge::ImageMosaicer
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:id __id) 0)
    ((:uav __uav) 0)
    ((:grafo __grafo) 0)
    ((:edges __edges) (make-array 0 :initial-element 0 :element-type :integer))
    ((:tx __tx) 0.0)
    ((:ty __ty) 0.0)
    ((:theta __theta) 0.0)
    ((:scale __scale) 0.0)
    ((:theta_offset __theta_offset) 0.0)
    ((:latitude __latitude) 0.0)
    ((:longitude __longitude) 0.0)
    ((:altitude __altitude) 0.0)
    ((:heading __heading) 0.0)
    ((:pitch __pitch) 0.0)
    ((:roll __roll) 0.0)
    ((:camera_type __camera_type) "")
    ((:filename __filename) "")
    ((:image __image) (instance sensor_msgs::CompressedImage :init))
    )
   (send-super :init)
   (setq _header __header)
   (setq _id (round __id))
   (setq _uav (round __uav))
   (setq _grafo (round __grafo))
   (setq _edges __edges)
   (setq _tx (float __tx))
   (setq _ty (float __ty))
   (setq _theta (float __theta))
   (setq _scale (float __scale))
   (setq _theta_offset (float __theta_offset))
   (setq _latitude (float __latitude))
   (setq _longitude (float __longitude))
   (setq _altitude (float __altitude))
   (setq _heading (float __heading))
   (setq _pitch (float __pitch))
   (setq _roll (float __roll))
   (setq _camera_type (string __camera_type))
   (setq _filename (string __filename))
   (setq _image __image)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:id
   (&optional __id)
   (if __id (setq _id __id)) _id)
  (:uav
   (&optional __uav)
   (if __uav (setq _uav __uav)) _uav)
  (:grafo
   (&optional __grafo)
   (if __grafo (setq _grafo __grafo)) _grafo)
  (:edges
   (&optional __edges)
   (if __edges (setq _edges __edges)) _edges)
  (:tx
   (&optional __tx)
   (if __tx (setq _tx __tx)) _tx)
  (:ty
   (&optional __ty)
   (if __ty (setq _ty __ty)) _ty)
  (:theta
   (&optional __theta)
   (if __theta (setq _theta __theta)) _theta)
  (:scale
   (&optional __scale)
   (if __scale (setq _scale __scale)) _scale)
  (:theta_offset
   (&optional __theta_offset)
   (if __theta_offset (setq _theta_offset __theta_offset)) _theta_offset)
  (:latitude
   (&optional __latitude)
   (if __latitude (setq _latitude __latitude)) _latitude)
  (:longitude
   (&optional __longitude)
   (if __longitude (setq _longitude __longitude)) _longitude)
  (:altitude
   (&optional __altitude)
   (if __altitude (setq _altitude __altitude)) _altitude)
  (:heading
   (&optional __heading)
   (if __heading (setq _heading __heading)) _heading)
  (:pitch
   (&optional __pitch)
   (if __pitch (setq _pitch __pitch)) _pitch)
  (:roll
   (&optional __roll)
   (if __roll (setq _roll __roll)) _roll)
  (:camera_type
   (&optional __camera_type)
   (if __camera_type (setq _camera_type __camera_type)) _camera_type)
  (:filename
   (&optional __filename)
   (if __filename (setq _filename __filename)) _filename)
  (:image
   (&rest __image)
   (if (keywordp (car __image))
       (send* _image __image)
     (progn
       (if __image (setq _image (car __image)))
       _image)))
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; int32 _id
    4
    ;; int32 _uav
    4
    ;; int32 _grafo
    4
    ;; int32[] _edges
    (* 4    (length _edges)) 4
    ;; float32 _tx
    4
    ;; float32 _ty
    4
    ;; float32 _theta
    4
    ;; float32 _scale
    4
    ;; float32 _theta_offset
    4
    ;; float64 _latitude
    8
    ;; float64 _longitude
    8
    ;; float64 _altitude
    8
    ;; float64 _heading
    8
    ;; float64 _pitch
    8
    ;; float64 _roll
    8
    ;; string _camera_type
    4 (length _camera_type)
    ;; string _filename
    4 (length _filename)
    ;; sensor_msgs/CompressedImage _image
    (send _image :serialization-length)
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; int32 _id
       (write-long _id s)
     ;; int32 _uav
       (write-long _uav s)
     ;; int32 _grafo
       (write-long _grafo s)
     ;; int32[] _edges
     (write-long (length _edges) s)
     (dotimes (i (length _edges))
       (write-long (elt _edges i) s)
       )
     ;; float32 _tx
       (sys::poke _tx (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _ty
       (sys::poke _ty (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _theta
       (sys::poke _theta (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _scale
       (sys::poke _scale (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _theta_offset
       (sys::poke _theta_offset (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float64 _latitude
       (sys::poke _latitude (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _longitude
       (sys::poke _longitude (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _altitude
       (sys::poke _altitude (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _heading
       (sys::poke _heading (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _pitch
       (sys::poke _pitch (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _roll
       (sys::poke _roll (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; string _camera_type
       (write-long (length _camera_type) s) (princ _camera_type s)
     ;; string _filename
       (write-long (length _filename) s) (princ _filename s)
     ;; sensor_msgs/CompressedImage _image
       (send _image :serialize s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; int32 _id
     (setq _id (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _uav
     (setq _uav (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _grafo
     (setq _grafo (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32[] _edges
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _edges (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _edges i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;; float32 _tx
     (setq _tx (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _ty
     (setq _ty (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _theta
     (setq _theta (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _scale
     (setq _scale (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _theta_offset
     (setq _theta_offset (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float64 _latitude
     (setq _latitude (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _longitude
     (setq _longitude (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _altitude
     (setq _altitude (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _heading
     (setq _heading (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _pitch
     (setq _pitch (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _roll
     (setq _roll (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; string _camera_type
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _camera_type (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; string _filename
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _filename (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; sensor_msgs/CompressedImage _image
     (send _image :deserialize buf ptr-) (incf ptr- (send _image :serialization-length))
   ;;
   self)
  )

(setf (get airsim_bridge::ImageMosaicer :md5sum-) "b1b370e85b8d0c6f3643c249518d4ae9")
(setf (get airsim_bridge::ImageMosaicer :datatype-) "airsim_bridge/ImageMosaicer")
(setf (get airsim_bridge::ImageMosaicer :definition-)
      "Header header
int32 id
int32 uav
int32 grafo
int32[] edges
float32 tx
float32 ty
float32 theta
float32 scale
float32 theta_offset
float64 latitude
float64 longitude
float64 altitude
float64 heading
float64 pitch
float64 roll
string camera_type
string  filename
sensor_msgs/CompressedImage image

================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
# 0: no frame
# 1: global frame
string frame_id

================================================================================
MSG: sensor_msgs/CompressedImage
# This message contains a compressed image

Header header        # Header timestamp should be acquisition time of image
                     # Header frame_id should be optical frame of camera
                     # origin of frame should be optical center of cameara
                     # +x should point to the right in the image
                     # +y should point down in the image
                     # +z should point into to plane of the image

string format        # Specifies the format of the data
                     #   Acceptable values:
                     #     jpeg, png
uint8[] data         # Compressed image buffer

")



(provide :airsim_bridge/ImageMosaicer "b1b370e85b8d0c6f3643c249518d4ae9")


