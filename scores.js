// Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyCahej0453N9l26zjuHkXbGyMTyFTgJcw0",
  authDomain: "splittingcomets.firebaseapp.com",
  projectId: "splittingcomets",
  storageBucket: "splittingcomets.firebasestorage.app",
  messagingSenderId: "280991593341",
  appId: "1:280991593341:web:b334e772452e026fcf1214",
  measurementId: "G-18ESZSW9ME"
};

// Initialize Firebase
firebase.initializeApp(firebaseConfig);
const db = firebase.firestore();

// Function to check if the current score is a new high score and save it
function updateHighScore(score, difficulty) {
  console.log(`Checking high score: ${score} points, difficulty: ${difficulty}`);
  
  // Ensure difficulty is a valid string
  if (!difficulty || difficulty === "undefined") {
    difficulty = "medium"; // Default to medium difficulty
  }
  
  // Use a standardized document ID for the high score
  const highScoreId = `highscore_${difficulty}`;
  
  // Reference to the high score document
  const highScoreRef = db.collection('highscores').doc(highScoreId);
  
  // Get the current high score
  return highScoreRef.get()
    .then(doc => {
      let isNewHighScore = false;
      
      if (doc.exists) {
        const currentHighScore = doc.data().score;
        console.log(`Current high score for ${difficulty}: ${currentHighScore}`);
        
        // Check if this is a new high score
        if (score > currentHighScore) {
          isNewHighScore = true;
          console.log(`New high score! ${score} > ${currentHighScore}`);
        } else {
          console.log(`Not a new high score. ${score} <= ${currentHighScore}`);
          return { success: true, isNewHighScore: false, highScore: currentHighScore };
        }
      } else {
        // No high score exists yet for this difficulty
        isNewHighScore = true;
        console.log(`First high score for ${difficulty}: ${score}`);
      }
      
      // If it's a new high score, save it
      if (isNewHighScore) {
        const scoreData = {
          score: score,
          difficulty: difficulty,
          timestamp: firebase.firestore.FieldValue.serverTimestamp(),
          dateAchieved: new Date().toISOString()
        };
        
        return highScoreRef.set(scoreData)
          .then(() => {
            console.log(`High score updated: ${score}`);
            return { success: true, isNewHighScore: true, highScore: score };
          });
      }
    })
    .catch(error => {
      console.error("Error updating high score:", error);
      return { success: false, error: error.message };
    });
}

// Function to get the current high score for a difficulty level
function getHighScore(difficulty = "medium") {
  const highScoreRef = db.collection('highscores').doc(`highscore_${difficulty}`);
  
  return highScoreRef.get()
    .then(doc => {
      if (doc.exists) {
        const highScoreData = doc.data();
        console.log(`Retrieved high score for ${difficulty}: ${highScoreData.score}`);
        return { 
          score: highScoreData.score, 
          date: highScoreData.dateAchieved 
        };
      } else {
        console.log(`No high score found for ${difficulty}`);
        return { score: 0, date: null };
      }
    })
    .catch(error => {
      console.error("Error getting high score:", error);
      return { score: 0, date: null, error: error.message };
    });
}