const express = require('express');
const mysql = require('mysql2');
const path = require('path');
const app = express();
const PORT = process.env.PORT || 3400;

// Database connection-a
const db = mysql.createConnection({
  host: '127.0.0.1',   // pulna hujnq, 2 chasa za db-to deto e tupo i oshte 2 za tva tupo ime
  user: 'root',
  password: 'Manchester United',
  database: 'novashema',
  port: 3310
});

db.connect((err) => {
  if (err) {
    console.error('Error connecting to the database:', err.stack);
    process.exit(1);
  }
  console.log('Connected to MySQL database');
});

// da se bara jason-a
app.use(express.json());

// dostup do front enda
app.use(express.static(path.join(__dirname, 'public')));

// API routing za save-vane na datata
app.post('/api/saveUser', (req, res) => {
  const { name, age, school } = req.body;
  const sql = 'INSERT INTO users (name, age, school) VALUES (?, ?, ?)';
  db.query(sql, [name, age, school], (err, result) => {
    if (err) {
      console.error('Error saving data:', err);
      res.status(500).send('Error saving data');
    } else {
      res.send('Data saved successfully');
    }
  });
});

// Staring the server
app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});