// Import functions from the Firebase SDK
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.15.0/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/9.15.0/firebase-database.js";

// Your web app's Firebase configuration
const firebaseConfig = {
    apiKey: "AIzaSyD8gCGOgc6yyzPxCmKuOlVFt6f148xjYuU", // From Firebase Project Settings [cite: 237]
    authDomain: "smart-ai-helmet-5b9f4.firebaseapp.com", // From Firebase Project Settings [cite: 238]
    databaseURL: "https://smart-ai-helmet-5b9f4-default-rtdb.firebaseio.com", // Your Database URL [cite: 239]
    storageBucket: "smart-ai-helmet-5b9f4.firebasestorage.app",
    projectId: "smart-ai-helmet-5b9f4", // From Firebase Project Settings [cite: 240]
};

// Initialize Firebase
const app = initializeApp(firebaseConfig); [cite: 245]
const database = getDatabase(app); [cite: 246]

// Get references to HTML elements
const statusEl = document.getElementById('status');
const gMagEl = document.getElementById('g-magnitude');
const heartRateEl = document.getElementById('heart-rate');

// Listen for data changes in Firebase
const helmetRef = ref(database, '/helmet');
onValue(helmetRef, (snapshot) => {
    const data = snapshot.val();
    if (data) {
        statusEl.textContent = data.status;
        gMagEl.textContent = data.acceleration?.magnitude.toFixed(2);
        heartRateEl.textContent = data.heartRate?.bpm;

        // Add a visual alert for a crash
        if (data.status === "Crash Detected") {
            document.body.style.backgroundColor = '#d9534f';
        } else {
            document.body.style.backgroundColor = '#f4f4f4';
        }
    }
});