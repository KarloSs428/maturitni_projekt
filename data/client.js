$(function(){
    let index = 0;
    function addGroup(index = 0, obj = {start:'', stop:''}) {
        return `
        <div class="form-group row p-1 group-time" id="group-${index}">
            <h3>Start</h3>
            <div class="col-sm-5">
                <input type="time" class="form-control" required name="start" id="start-${index}" value="${obj.start}">
            </div>
            <h3>Stop</h3>
            <div class="col-sm-5">
                <input type="time" class="form-control" required name="stop" id="stop-${index}" value="${obj.stop}">
            </div>
        </div>
        `;
    }
    
    $('#append').on('click', function() { 
        $('#times').append(addGroup(index));
        index++;
    });

    let data = [];

    function validateData(obj) {
        return (obj.start.replace(':', '') < obj.stop.replace(':', '')); 
    }

    function get() {
        $.ajax({
            url: 'http://localhost:3000/api/times',
            type: 'GET', 
            dataType: 'json', 
            cache: false, 
            success: (data, textStatus, xhr) => {
                $('#times').html(''); 
                data.forEach(obj => {
                    $('#times').append(addGroup(index, obj));
                    index++;
                });                
            },
            error: (xhr, textStatus, errorThrown) => {
                console.log(errorThrown);
            }
        })
    }

    function send(data) {
        $.ajax({
            url: 'http://localhost:3000/api/times',
            type: 'PUT',
            data: data,
            dataType: 'json',            
            contentType: 'application/json',
            success: (data, textStatus, xhr) => {
                console.log(textStatus);
                console.log(data);
                get();
            },
            error: (xhr, textStatus, errorThrown) => {
                console.log(errorThrown);
            }
        })
    }
    
    $('#send').on('click', function() {
        let valid = true;
        data = [];
        $('#error').addClass('d-none');
        $('div.group-time').each(function(key, obj) {
            let timeGroup = {}
            $(obj).find('input').each(function(key, input) {
                timeGroup[input.name] = input.value;
            });
            if (!validateData(timeGroup)) {
                $(obj).addClass('error');
                valid = false;
            } else {
                $(obj).removeClass('error');
            }
            if (valid) data.push(timeGroup);
        });
        if (!valid) {
           $('#error').removeClass('d-none');
        } else {
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
    get();

})






