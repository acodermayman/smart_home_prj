import express from 'express';
import { getAllDeviceStatus } from '../controllers/controllers.js';

const router = express.Router();

router.get('/deviceStatus', getAllDeviceStatus);

export default router;
