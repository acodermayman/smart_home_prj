import mysql2 from 'mysql2';

export const connectDB = mysql2.createPool({
    host: 'localhost',
    user: 'root',
    password: '',
    database: 'smart_home',
}).promise();
