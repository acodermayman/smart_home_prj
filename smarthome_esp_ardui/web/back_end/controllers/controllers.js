import { connectDB as DB} from "../config/db.js";

export const getAllDeviceStatus = async (req, res) => {
    try {
        const [data] = await DB.query("SELECT * FROM devices_status");
        if(!data){
            return res.status(404).json({ success: false, message: "No device status found" });
        }
        return res.status(200).json({ success: true, message: "Device status", data });
     }
    catch (err) {
        console.error(err);
        return res.status(500).json({ success: false, message: err.message });
    }
};
