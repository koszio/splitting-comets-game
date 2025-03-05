// Simple script to test the local API server
const http = require('http');

// Test POST request to add a score
function testPostScore() {
  console.log('Testing POST /scores endpoint...');
  
  const data = JSON.stringify({
    username: 'TestUser',
    difficulty: 'medium',
    score: 500
  });
  
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
    console.log(`Status Code: ${res.statusCode}`);
    
    res.on('data', (chunk) => {
      console.log(`Response: ${chunk.toString()}`);
    });
    
    // After successful POST, test the GET endpoint
    res.on('end', () => {
      setTimeout(testGetScores, 500); // Wait a bit before testing GET
    });
  });
  
  req.on('error', (error) => {
    console.error(`Error: ${error.message}`);
  });
  
  req.write(data);
  req.end();
}

// Test GET request to fetch scores
function testGetScores() {
  console.log('\nTesting GET /scores endpoint...');
  
  const options = {
    hostname: 'localhost',
    port: 3000,
    path: '/scores',
    method: 'GET'
  };
  
  const req = http.request(options, (res) => {
    console.log(`Status Code: ${res.statusCode}`);
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
        console.log('Raw response:', rawData);
      }
    });
  });
  
  req.on('error', (error) => {
    console.error(`Error: ${error.message}`);
  });
  
  req.end();
}

// Run the tests
testPostScore();