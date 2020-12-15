/* Načtení vestavěného modulu fs (file system) - umožňuje práci se souborovým systémem v Node JS */
const fs = require("fs");
/* Načtení externího modulu joi (musí být nejprve naistalován pomocí npm install joi).
   Usnadňuje ověření správného zadání dat (validaci).  
 */
const Joi = require("joi");
/* Načtení externího modulu cors (musí být nejprve naistalován pomocí npm install cors).
   Umožní serveru zpracovat i požadavky, které přicházejí z jiné domény a které by mohly 
   být vyhodnoceny jako potenciálně nebezpečné.
   Podrobněji viz https://cs.wikipedia.org/wiki/CORS
*/
const cors = require("cors");
/* Načtení externího modulu express - oblíbený framework pro tvorbu webových aplikací 
   v Node JS. */
const express = require("express");
/* Vytvoření základního objektu aplikace */
const app = express();
/* Povolí v aplikaci zpracování dat ve formátu JSON */
app.use(express.json());
/* Povolí v aplikaci zpracovat požadavky přicházející z jiné domény */
app.use(cors());

/** Pomocné funkce **/
/* Funkce umožní validaci dat určitého objektu (zde časového záznamu) */
function validateData(dataObject) {
  /* Validační schéma nastavuje pravidla platná pro jednotlivé atributy objektu. 
     Příklady různých možností tvorby schémat knihovny Joi naleznete třeba zde: https://www.digitalocean.com/community/tutorials/node-api-schema-validation-with-joi 
  */ 
  const schema = {
    /* start - čas musí být ve správném formátu - 00:00 až 23:59 */
    start: Joi.string().regex(/^([01]\d|2[0-3]):[0-5]\d$/).required(),
    stop: Joi.string().regex(/^([01]\d|2[0-3]):[0-5]\d$/).required(),
    /* date - platné datum, nanejvýš 31. 12. 2021, ve formátu yyyy-mm-dd */  
    date: Joi.date().max('12-31-2021'),
  };
  return Joi.validate(dataObject, schema);
}

/* Funkce slouží k asynchronnímu zápisu dat ve formě pole objektů do souboru ve formátu JSON umístěného v pathToFile */
function writeJSON(arrayObj, pathToFile) {
  /* Pole objektů je převedeno na řetězec ve formátu JSON, hodnota 2 v třetím parametru znamená odsazení */
  let data = JSON.stringify(arrayObj, null, 2);
  /* Metoda writeFile modulu fs zapíše do souboru pathToFile připravená data */
  try {
    /* Metoda JSON.parse vytvoří pole objektů z dat získaných načtením souboru */
    fs.writeFileSync(pathToFile, data, { encoding: "utf8", flag: "w", mode: 0o666 });
    /* Vypíše se hlášení o úspěšně provedené akci do konzole */
    console.log(`Data was saved successfully to ${pathToFile}`);
  } catch(err) {
    /* V případě chyby se vypíše hláška do konzole */
    console.log(`Data couldn't be saved! Error: ${err}`);
  }  
}

/* Funkce slouží k synchronnímu načtení dat do pole objektů ze souboru ve formátu JSON umístěného v pathToFile */
function readJSON(pathToFile) {
  /* Ošetření výjimky - dojde-li k nějaké chybě v bloku try, je chyba zachycena (a ošetřena) v bloku catch. 
     K výjimce může dojít například po neúspěšném čtení ze souboru nebo parsování dat z formátu JSON. 
  */
  try {
    /* Metoda JSON.parse vytvoří pole objektů z dat získaných načtením souboru */
    let data = JSON.parse(fs.readFileSync(pathToFile, 'utf8'));
    /* Vypíše se hlášení o úspěšně provedené akci do konzole */
    console.log(`Data was read successfully from file ${pathToFile}`);
    /* Funkce vrátí načtená data */
    return data;  
  } catch(err) {
    /* V případě chyby se vypíše hláška do konzole */
    console.log(`Data couldn't be read! Error: ${err}`);
  }
}

/* Request: použití metody GET, URL adresy /:
   Response: HTML stránka  */
app.get("/", (req, res) => {
  res.send("<h1>Úvodní stránka - REST API</h1>");
});

/* Načtení všech záznamů (data o všech časech)
   Request: použití metody GET, URL adresy /api/movies:
   Response: posílá obsah proměnné times (pole objektů) - údaje o všech časech  */
app.get("/api/times", (req, res) => {
  res.send(readJSON("./times.json"));
});

/* Zapisování/aktualizace záznamů 
   Request: použití metody PUT, URL adresy /api/times
   Response: error / success   */
app.put("/api/times", (req, res) => {
  let data = req.body;
  times = [];
  data.forEach(function(obj) {
    /* Pomocí funkce validateData zkontroluje data odeslaná v těle požadavku; případná chyba se uloží ve formě objektu. */
    let { error } = validateData(obj);
    /* Jestliže došlo k chybě při validaci dat (např. nesprávné datum) */
    if (error) {
      /* v odpovědi serveru se pošle zpráva 400 a informace o detailech chyby */
      res.status(400).send(error.details[0].message);
    } else {
      /* V případě, že validace dat proběhla úspěšně, vytvoří se objekt time s potřebnými atributy a z těla požadavku se do něj uloží všechna data */
      let time = {
        start: obj.start,
        stop: obj.stop,
        date: obj.date,
      };
      /* Přidá nový časový záznam do pole */
      times.push(time);
      /* Zapíše aktuální data do souboru JSON */
    }
  });
  /* Zapíše data do souboru */
  writeJSON(times, 'times.json');
  /* Jako odpověď pošle úspěšně vložený objekt */
  res.send(readJSON("./times.json"));
});


/* Serverová aplikace je spuštěna a naslouchá na portu 3000 */
app.listen(3000, () => console.log("Listening on port 3000..."));
