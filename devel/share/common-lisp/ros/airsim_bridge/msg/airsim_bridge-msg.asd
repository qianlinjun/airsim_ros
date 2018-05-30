
(cl:in-package :asdf)

(defsystem "airsim_bridge-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :sensor_msgs-msg
               :std_msgs-msg
)
  :components ((:file "_package")
    (:file "ImageMosaicer" :depends-on ("_package_ImageMosaicer"))
    (:file "_package_ImageMosaicer" :depends-on ("_package"))
    (:file "ImageMosaicer" :depends-on ("_package_ImageMosaicer"))
    (:file "_package_ImageMosaicer" :depends-on ("_package"))
  ))