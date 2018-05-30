// Auto-generated. Do not edit!

// (in-package airsim_bridge.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let sensor_msgs = _finder('sensor_msgs');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class ImageMosaicer {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.id = null;
      this.uav = null;
      this.grafo = null;
      this.edges = null;
      this.tx = null;
      this.ty = null;
      this.theta = null;
      this.scale = null;
      this.theta_offset = null;
      this.latitude = null;
      this.longitude = null;
      this.altitude = null;
      this.heading = null;
      this.pitch = null;
      this.roll = null;
      this.camera_type = null;
      this.filename = null;
      this.image = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('id')) {
        this.id = initObj.id
      }
      else {
        this.id = 0;
      }
      if (initObj.hasOwnProperty('uav')) {
        this.uav = initObj.uav
      }
      else {
        this.uav = 0;
      }
      if (initObj.hasOwnProperty('grafo')) {
        this.grafo = initObj.grafo
      }
      else {
        this.grafo = 0;
      }
      if (initObj.hasOwnProperty('edges')) {
        this.edges = initObj.edges
      }
      else {
        this.edges = [];
      }
      if (initObj.hasOwnProperty('tx')) {
        this.tx = initObj.tx
      }
      else {
        this.tx = 0.0;
      }
      if (initObj.hasOwnProperty('ty')) {
        this.ty = initObj.ty
      }
      else {
        this.ty = 0.0;
      }
      if (initObj.hasOwnProperty('theta')) {
        this.theta = initObj.theta
      }
      else {
        this.theta = 0.0;
      }
      if (initObj.hasOwnProperty('scale')) {
        this.scale = initObj.scale
      }
      else {
        this.scale = 0.0;
      }
      if (initObj.hasOwnProperty('theta_offset')) {
        this.theta_offset = initObj.theta_offset
      }
      else {
        this.theta_offset = 0.0;
      }
      if (initObj.hasOwnProperty('latitude')) {
        this.latitude = initObj.latitude
      }
      else {
        this.latitude = 0.0;
      }
      if (initObj.hasOwnProperty('longitude')) {
        this.longitude = initObj.longitude
      }
      else {
        this.longitude = 0.0;
      }
      if (initObj.hasOwnProperty('altitude')) {
        this.altitude = initObj.altitude
      }
      else {
        this.altitude = 0.0;
      }
      if (initObj.hasOwnProperty('heading')) {
        this.heading = initObj.heading
      }
      else {
        this.heading = 0.0;
      }
      if (initObj.hasOwnProperty('pitch')) {
        this.pitch = initObj.pitch
      }
      else {
        this.pitch = 0.0;
      }
      if (initObj.hasOwnProperty('roll')) {
        this.roll = initObj.roll
      }
      else {
        this.roll = 0.0;
      }
      if (initObj.hasOwnProperty('camera_type')) {
        this.camera_type = initObj.camera_type
      }
      else {
        this.camera_type = '';
      }
      if (initObj.hasOwnProperty('filename')) {
        this.filename = initObj.filename
      }
      else {
        this.filename = '';
      }
      if (initObj.hasOwnProperty('image')) {
        this.image = initObj.image
      }
      else {
        this.image = new sensor_msgs.msg.CompressedImage();
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type ImageMosaicer
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [id]
    bufferOffset = _serializer.int32(obj.id, buffer, bufferOffset);
    // Serialize message field [uav]
    bufferOffset = _serializer.int32(obj.uav, buffer, bufferOffset);
    // Serialize message field [grafo]
    bufferOffset = _serializer.int32(obj.grafo, buffer, bufferOffset);
    // Serialize message field [edges]
    bufferOffset = _arraySerializer.int32(obj.edges, buffer, bufferOffset, null);
    // Serialize message field [tx]
    bufferOffset = _serializer.float32(obj.tx, buffer, bufferOffset);
    // Serialize message field [ty]
    bufferOffset = _serializer.float32(obj.ty, buffer, bufferOffset);
    // Serialize message field [theta]
    bufferOffset = _serializer.float32(obj.theta, buffer, bufferOffset);
    // Serialize message field [scale]
    bufferOffset = _serializer.float32(obj.scale, buffer, bufferOffset);
    // Serialize message field [theta_offset]
    bufferOffset = _serializer.float32(obj.theta_offset, buffer, bufferOffset);
    // Serialize message field [latitude]
    bufferOffset = _serializer.float64(obj.latitude, buffer, bufferOffset);
    // Serialize message field [longitude]
    bufferOffset = _serializer.float64(obj.longitude, buffer, bufferOffset);
    // Serialize message field [altitude]
    bufferOffset = _serializer.float64(obj.altitude, buffer, bufferOffset);
    // Serialize message field [heading]
    bufferOffset = _serializer.float64(obj.heading, buffer, bufferOffset);
    // Serialize message field [pitch]
    bufferOffset = _serializer.float64(obj.pitch, buffer, bufferOffset);
    // Serialize message field [roll]
    bufferOffset = _serializer.float64(obj.roll, buffer, bufferOffset);
    // Serialize message field [camera_type]
    bufferOffset = _serializer.string(obj.camera_type, buffer, bufferOffset);
    // Serialize message field [filename]
    bufferOffset = _serializer.string(obj.filename, buffer, bufferOffset);
    // Serialize message field [image]
    bufferOffset = sensor_msgs.msg.CompressedImage.serialize(obj.image, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type ImageMosaicer
    let len;
    let data = new ImageMosaicer(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [id]
    data.id = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [uav]
    data.uav = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [grafo]
    data.grafo = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [edges]
    data.edges = _arrayDeserializer.int32(buffer, bufferOffset, null)
    // Deserialize message field [tx]
    data.tx = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [ty]
    data.ty = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [theta]
    data.theta = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [scale]
    data.scale = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [theta_offset]
    data.theta_offset = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [latitude]
    data.latitude = _deserializer.float64(buffer, bufferOffset);
    // Deserialize message field [longitude]
    data.longitude = _deserializer.float64(buffer, bufferOffset);
    // Deserialize message field [altitude]
    data.altitude = _deserializer.float64(buffer, bufferOffset);
    // Deserialize message field [heading]
    data.heading = _deserializer.float64(buffer, bufferOffset);
    // Deserialize message field [pitch]
    data.pitch = _deserializer.float64(buffer, bufferOffset);
    // Deserialize message field [roll]
    data.roll = _deserializer.float64(buffer, bufferOffset);
    // Deserialize message field [camera_type]
    data.camera_type = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [filename]
    data.filename = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [image]
    data.image = sensor_msgs.msg.CompressedImage.deserialize(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    length += 4 * object.edges.length;
    length += object.camera_type.length;
    length += object.filename.length;
    length += sensor_msgs.msg.CompressedImage.getMessageSize(object.image);
    return length + 92;
  }

  static datatype() {
    // Returns string type for a message object
    return 'airsim_bridge/ImageMosaicer';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'b1b370e85b8d0c6f3643c249518d4ae9';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    Header header
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
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new ImageMosaicer(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.id !== undefined) {
      resolved.id = msg.id;
    }
    else {
      resolved.id = 0
    }

    if (msg.uav !== undefined) {
      resolved.uav = msg.uav;
    }
    else {
      resolved.uav = 0
    }

    if (msg.grafo !== undefined) {
      resolved.grafo = msg.grafo;
    }
    else {
      resolved.grafo = 0
    }

    if (msg.edges !== undefined) {
      resolved.edges = msg.edges;
    }
    else {
      resolved.edges = []
    }

    if (msg.tx !== undefined) {
      resolved.tx = msg.tx;
    }
    else {
      resolved.tx = 0.0
    }

    if (msg.ty !== undefined) {
      resolved.ty = msg.ty;
    }
    else {
      resolved.ty = 0.0
    }

    if (msg.theta !== undefined) {
      resolved.theta = msg.theta;
    }
    else {
      resolved.theta = 0.0
    }

    if (msg.scale !== undefined) {
      resolved.scale = msg.scale;
    }
    else {
      resolved.scale = 0.0
    }

    if (msg.theta_offset !== undefined) {
      resolved.theta_offset = msg.theta_offset;
    }
    else {
      resolved.theta_offset = 0.0
    }

    if (msg.latitude !== undefined) {
      resolved.latitude = msg.latitude;
    }
    else {
      resolved.latitude = 0.0
    }

    if (msg.longitude !== undefined) {
      resolved.longitude = msg.longitude;
    }
    else {
      resolved.longitude = 0.0
    }

    if (msg.altitude !== undefined) {
      resolved.altitude = msg.altitude;
    }
    else {
      resolved.altitude = 0.0
    }

    if (msg.heading !== undefined) {
      resolved.heading = msg.heading;
    }
    else {
      resolved.heading = 0.0
    }

    if (msg.pitch !== undefined) {
      resolved.pitch = msg.pitch;
    }
    else {
      resolved.pitch = 0.0
    }

    if (msg.roll !== undefined) {
      resolved.roll = msg.roll;
    }
    else {
      resolved.roll = 0.0
    }

    if (msg.camera_type !== undefined) {
      resolved.camera_type = msg.camera_type;
    }
    else {
      resolved.camera_type = ''
    }

    if (msg.filename !== undefined) {
      resolved.filename = msg.filename;
    }
    else {
      resolved.filename = ''
    }

    if (msg.image !== undefined) {
      resolved.image = sensor_msgs.msg.CompressedImage.Resolve(msg.image)
    }
    else {
      resolved.image = new sensor_msgs.msg.CompressedImage()
    }

    return resolved;
    }
};

module.exports = ImageMosaicer;
