$(function(){
    /* Inicializace proměnné index, která unikátně identifikuje novou skupinu prvků */
    let index = 0;
    /* Funkce, která přidá skupinu prvků do formuláře */
    function addGroup(index = 0, obj = {start:'', stop:'', date:''}) {
        return `
        <div class="form-group row p-1 group-time" id="group-${index}">
            <label>Start</label>
            <div class="col-sm-3">
                <input type="time" class="form-control" required name="start" id="start-${index}" value="${obj.start}">
            </div>
            <label>Stop</label>
            <div class="col-sm-3">
                <input type="time" class="form-control" required name="stop" id="stop-${index}" value="${obj.stop}">
            </div>
            <label>Den</label>
            <div class="col-sm-3">
                <input type="date" class="form-control" required name="date" id="date-${index}" value="${obj.date}">
            </div>
            <div class="col-sm-3">
                <button type="button" class="btn btn-danger delete" id="delete-${index}">Smazat</button>
            </div>
        </div>
        `;
    }
    
    /* Akce po kliknutí na tlačítko Přidat nový čas */
    $('#append').on('click', function() { 
        /* Přidá skupinu prvků jako nový blok do formuláře */
        $('#times').append(addGroup(index));
        /* Ošetření akce kliknutí na tlačítko Smazat - smaže se vybraná skupina prvků */
        $('.delete').on('click', function() { 
            /* Příklad traverzování - vyhledá se a odstraní celý element - 
            předek tlačítka Smazat, který je oddílem s třídou group-time */
            $(this).parents('div.group-time').remove();        
        });   
        /* Index se po přidání prvku zvýší, aby se zajistila jeho unikátnost */ 
        index++;
    });

    /* Pole pro uložení dat z formuláře */
    let data = [];

    /* Validační funkce pro ověření platnosti zadaných údajů */
    function validateData(obj) {
        /* Z obou časových stringů odebere dvojtečku a porovná, zda je konečný údaj větší */
        /* Musí být zadáno také neprázdné datum */
        return (obj.start.replace(':', '') < obj.stop.replace(':', '')) && obj.date; 
    }

    function get() {
        /* Metoda JQuery, která umožňuje použití AJAX */
        $.ajax({
            /* Adresa požadavku */
            url: 'http://localhost:3000/api/times',
            type: 'GET', // typ použité metody
            dataType: 'json', // formát přijímaných dat
            cache: false, // možnost využití vyrovnávací paměti
            /* V případě úspěšné odpovědi serveru - její součástí jsou posílaná data, stavová zpráva a xhr (XMLHTTPRequest objekt) */
            success: (data, textStatus, xhr) => {
                /* Vyčistí obsah elementu s id="times" */
                $('#times').html(''); 
                /* Prochází všechna data (pole objektů) */
                data.forEach(obj => {
                    $('#times').append(addGroup(index, obj));
                    /* Ošetření akce kliknutí na tlačítko Smazat - smaže se vybraná skupina prvků */
                    $('.delete').on('click', function() { 
                        /* Příklad traverzování - vyhledá se a odstraní celý element - 
                        předek tlačítka Smazat, který je oddílem s třídou group-time */
                        $(this).parents('div.group-time').remove();        
                    });   
                    /* Index se po přidání prvku zvýší, aby se zajistila jeho unikátnost */ 
                    index++;
                });                
            },
            /* V případě chyby se v konzoli objeví chybové hlášení */
            error: (xhr, textStatus, errorThrown) => {
                console.log(errorThrown);
            }
        })
    }


    /* Pošle data na server prostřednictvím metody PUT */
    function send(data) {
        $.ajax({
            /* URL serveru */
            url: 'http://localhost:3000/api/times',
            /* Metoda PUT */
            type: 'PUT',
            /* Předávaná data */
            data: data,
            /* Použitý datový typ - JSON */
            dataType: 'json',            
            /* MIME typ */
            contentType: 'application/json',
            /* Odpověď po úspěšném přijetí dat */
            success: (data, textStatus, xhr) => {
                console.log(textStatus);
                console.log(data);
                get();
            },
            /* Odpověď po chybě */
            error: (xhr, textStatus, errorThrown) => {
                console.log(errorThrown);
            }
        })
    }
    

    /* Ošetření události kliknutí na tlačítko Aktualizovat data */
    $('#send').on('click', function() {
        /* Nastavení kontrolní proměnné valid - na začátku předpokládáme platný formulář */
        let valid = true;
        /* Vyprázdění pole data */
        data = [];
        /* Přidání třídy d-none - zneviditelnění divu s id=error */
        $('#error').addClass('d-none');
        /* Prochází všechny objekty s třídou .group-time  */
        $('div.group-time').each(function(key, obj) {
            /* Pomocný objekt pro uložení dat z jedné skupiny */
            let timeGroup = {}
            /* Prochází všechny elementy input ve skupině */
            $(obj).find('input').each(function(key, input) {
                /* Přidá do objektu timeGroup údaj s klíčem input.name a přiřadí mu hodnotu z formuláře */
                timeGroup[input.name] = input.value;
            });
            /* V případě, že byla ve skupině zjištěna neplatná data */
            if (!validateData(timeGroup)) {
                /* Změní styl skupiny pomocí třídy .error */
                $(obj).addClass('error');
                /* Nastaví příznak neplatnosti formuláře */
                valid = false;
            } else {
                /* Data ve skupině jsou platná, není třeba zdůrazňovat chybu */
                $(obj).removeClass('error');
            }
            /* Jsou-li data platná, vloží se objekt s údaji pro danou skupinu do pole data */
            if (valid) data.push(timeGroup);
        });
        if (!valid) {
           /* Když je formulář nevalidní, zviditelní se chybový alert */ 
           $('#error').removeClass('d-none');
        } else {
            /* JSON serializace dat z formuláře */
            send(JSON.stringify(data));
        }
    });

    function showTime() {
        var date = new Date();
        var h = date.getHours();
        var m = date.getMinutes();
        var s = date.getSeconds();
        var session = "AM";
        if (h == 0) {
            h = 12;
        }
        if (h > 12) {
            h = h - 12;
            session = "PM";
        }
        h = (h < 10) ? "0" + h : h;
        m = (m < 10) ? "0" + m : m;
        s = (s < 10) ? "0" + s : s;
        var time = h + ":" + m + ":" + s + " " + session;
        document.getElementById("aktualniCas").innerText = time;
        document.getElementById("aktualniCas").textContent = time;
        setTimeout(showTime, 1000);
    }

    showTime();

    /* Načtení dat ze serveru */
    get();

})






