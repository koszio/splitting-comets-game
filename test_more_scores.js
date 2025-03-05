// Script to add multiple scores to our SQLite database
const http = require('http');

// Sample test data with different difficulties and scores
const testScores = [
  { username: 'Alice', difficulty: 'easy', score: 300 },
  { username: 'Bob', difficulty: 'easy', score: 450 },
  { username: 'Charlie', difficulty: 'medium', score: 600 },
  { username: 'Diana', difficulty: 'medium', score: 550 },
  { username: 'Eve', difficulty: 'hard', score: 800 },
  { username: 'Frank', difficulty: 'hard', score: 750 },
  { username: 'George', difficulty: 'infinity', score: 1200 },
  { username: 'Hannah', difficulty: 'infinity', score: 1500 }
];

// Function to add a score
function addScore(scoreData, callback) {
  const data = JSON.stringify(scoreData);
  
  const options = {
    hostname: 'localhost',
    port: 3000,
    path: '/scores',
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Content-Length': data.length
    }
  };
  
  const req = http.request(options, (res) => {
    let responseData = '';
    
    res.on('data', (chunk) => {
      responseData += chunk;
    });
    
    res.on('end', () => {
      console.log(`Added score for ${scoreData.username}: ${scoreData.score} (${scoreData.difficulty})`);
      if (callback) callback();
    });
  });
  
  req.on('error', (error) => {
    console.error(`Error: ${error.message}`);
    if (callback) callback(error);
  });
  
  req.write(data);
  req.end();
}

// Add scores sequentially
function addScoresSequentially(scores, index = 0) {
  if (index >= scores.length) {
    console.log('All scores added successfully!');
    fetchAllScores();
    return;
  }
  
  addScore(scores[index], (error) => {
    if (error) {
      console.error('Error adding score, stopping sequence.');
      return;
    }
    
    // Add next score after a short delay
    setTimeout(() => {
      addScoresSequentially(scores, index + 1);
    }, 200);
  });
}

// Fetch all scores to verify
function fetchAllScores() {
  console.log('\nFetching all scores...');
  
  const options = {
    hostname: 'localhost',
    port: 3000,
    path: '/scores',
    method: 'GET'
  };
  
  const req = http.request(options, (res) => {
    let rawData = '';
    
    res.on('data', (chunk) => {
      rawData += chunk;
    });
    
    res.on('end', () => {
      try {
        const scores = JSON.parse(rawData);
        console.log('Scores received:');
        console.log(JSON.stringify(scores, null, 2));
      } catch (e) {
        console.error(`Error parsing response: ${e.message}`);
      }
    });
  });
  
  req.on('error', (error) => {
    console.error(`Error: ${error.message}`);
  });
  
  req.end();
}

// Start adding scores
addScoresSequentially(testScores);