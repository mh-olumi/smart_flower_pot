require('dotenv').config(); // Load environment variables from .env file

const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const path = require('path');


const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(cors({ origin: '*' })); // Enable CORS
app.use(bodyParser.json()); // Parse incoming JSON data
app.use(express.static(path.join(__dirname, 'public'))); // Serve static files

// MongoDB Configuration
const mongoUri = 'mongodb://root:1o4lIPLUvw2KoeDbKejbBh8V@qomiot2:27017/my-app?authSource=admin'; // MongoDB URI from Liara
const dbName = 'qomiot2';
let db;

// Connect to MongoDB
MongoClient.connect(mongoUri, { useNewUrlParser: true, useUnifiedTopology: true })
  .then((client) => {
    console.log('Connected to MongoDB');
    db = client.db(dbName);
  })
  .catch((err) => {
    console.error('Failed to connect to MongoDB:', err);
    process.exit(1); // Exit if connection fails
  });

// API endpoint to receive sensor data from ESP32
app.post('/api/sensor-data', async (req, res) => {
  const { temperature, humidity, lightValue } = req.body;

  if (
    typeof temperature === 'number' &&
    typeof humidity === 'number' &&
    typeof lightValue === 'number'
  ) {
    const sensorData = {
      temperature,
      humidity,
      lightValue,
      timestamp: new Date(),
    };

    try {
      const collection = db.collection('sensorData'); // Use 'sensorData' collection
      await collection.insertOne(sensorData); // Save data to MongoDB
      console.log('Received and saved sensor data:', sensorData);
      res.status(200).json({ message: 'Data saved successfully.' });
    } catch (err) {
      console.error('Error saving sensor data:', err);
      res.status(500).json({ message: 'Failed to save data.' });
    }
  } else {
    res.status(400).json({ message: 'Invalid data format.' });
  }
});

// API endpoint to get the latest sensor data
app.get('/api/sensor-data', async (req, res) => {
  try {
    const collection = db.collection('sensorData');
    const latestData = await collection.find().sort({ timestamp: -1 }).limit(1).toArray();
    res.status(200).json(latestData[0] || {});
  } catch (err) {
    console.error('Error fetching sensor data:', err);
    res.status(500).json({ message: 'Failed to fetch data.' });
  }
});

// Serve the frontend
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Start the server
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});
