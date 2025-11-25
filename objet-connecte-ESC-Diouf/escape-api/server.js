const express = require("express");
const fs = require("fs");
const app = express();
const PORT = 3005;

app.use(express.json());
app.use(require("cors")());
app.use(express.static("public"));

const DATA_FILE = "events.json";

// Création du fichier si inexistant
if (!fs.existsSync(DATA_FILE)) {
    fs.writeFileSync(DATA_FILE, JSON.stringify([]));
}



/** 
 * POST : Reçoit l’événement du ESP32
 */ 
app.post("/api/game/event", (req, res) => {
    const event = req.body;

    console.log("Reçu:", event);

    // Charge fichier existant
    const data = JSON.parse(fs.readFileSync(DATA_FILE));


    // Ajoute événement + date
    data.push({
        ...event,
        receivedAt: new Date().toISOString()
    });

    
    // Sauvegarde
    fs.writeFileSync(DATA_FILE, JSON.stringify(data, null, 2));

    res.json({ status: "ok", message: "Event saved" });
});



/**  
 * GET : Retourne tous les événements
 */
app.get("/api/game/events", (req, res) => {
    const data = JSON.parse(fs.readFileSync(DATA_FILE));
    res.json(data);
});



/** 
 * GET : Statistiques globales
 */
app.get("/api/game/stats", (req, res) => {
    const events = JSON.parse(fs.readFileSync(DATA_FILE));

    const pir = events.filter(e => e.event === "pir").length;
    const success = events.filter(e => e.event === "success").length;
    const failure = events.filter(e => e.event === "failure").length;
    const total = events.length;

    res.json({ total, pir, success, failure });
});



/**  
 * GET : Statistiques des boutons
 */
app.get("/api/game/stats/buttons", (req, res) => {
    const events = JSON.parse(fs.readFileSync(DATA_FILE));

    const btn = { rouge: 0, bleu: 0, vert: 0, jaune: 0 };

    events.forEach(e => {
        if (e.event === "button") {
            if (e.value === 1) btn.rouge++;
            if (e.value === 2) btn.bleu++;
            if (e.value === 3) btn.vert++;
            if (e.value === 4) btn.jaune++;
        }
    });

    res.json(btn);
});

app.listen(PORT, () => {
    console.log("API run sur http://localhost:" + PORT);
});
