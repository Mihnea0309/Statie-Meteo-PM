<h1>Stație Meteo</h1>

<h2>Obiective</h2>
Acest proiect a fost realizat pentru a măsura o serie de parametri, precum temperatura, umiditatea, presiunea și indicele UV. Sistemul are un panou LCD pentru a afișa aceste informații și, de asemenea, pentru a le compara cu datele în timp real de pe site-ul OpenWeather. Totodată, stația trimite informații în timp real către două site-uri: WeatherUnderground, unde mi-am creat propria stație și unde trimite informațiile, și ThingSpeak, unde compar datele stației cu cele de la OpenWeather.
<br />

<h2>Descrierea soluției</h2>
Am realizat stația meteo folosind componente accesibile, cum ar fi:

- Senzori de date: un senzor DHT22 pentru temperatură și umiditate, un senzor UV și un senzor BME280 pentru presiune
- Panou de afișare: un panou LCD TFT pentru a afișa informațiile necesare
- Unitate de control: un modul Wifi NodeMCU V3, ESP8266, programat pentru a procesa datele de la senzori, a le afișa pe LCD, a prelua date și a le trimite către site-urile menționate
- Trei baterii AA de 1.5V pentru alimentarea unității de control
- Suportul: o improvizație realizată dintr-o husă de telefon.
  
<h2>Realizarea proiectului:</h2>

- Mai întâi, am conectat senzorii de date și unitatea de control pentru a mă asigura că totul funcționează corect.
- Apoi, am conectat panoul LCD pentru a afișa informațiile către utilizator.
- Am lucrat la codul dorit pentru unitatea de control. Am gestionat datele primite, le-am trimis către site-uri și le-am afișat pe panou.
- La final, după ce am terminat de scris codul, am conectat unitatea de control la baterii de 1.5V pentru a funcționa de la distanță, fără a fi nevoie de un cablu USB.
  
<h2>Testarea soluției</h2>
Pentru a testa proiectul, am realizat următorii pași:
- Am încărcat codul pe placa NodeMCU.
- Odată încărcat, panoul a afișat informațiile dorite, atât de la senzorii mei, cât și de pe site-ul OpenWeather.
- Apoi, am verificat datele de la senzori folosind un termostat (deoarece am testat stația într-o cameră, iar datele OpenWeather sunt pentru exterior) și m-am asigurat că datele au fost trimise și către cele două site-uri dorite.
<p align="center">
  <img src="https://imgur.com/FzCbyC0.png" height="80%" width="80%" alt="Pașii de dezinfectare a discului"/>
</p>
<p align="center">
  <img src="https://imgur.com/1oFxHwP.png" height="80%" width="80%" alt="Pașii de dezinfectare a discului"/>
</p>

<h2>Limbaje utilizate</h2>

- <b>C</b> 

<h2>Mediile utilizate</h2>

- <b>Arduino IDE</b>

<h3><a href = "https://youtu.be/3CEIe9QNAU0">Video cu Stația Meteo</a></h3>
