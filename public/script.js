function submitData() {
    const name = document.getElementById('name').value;
    const age = document.getElementById('age').value;
    const school = document.getElementById('school').value;

    fetch('/api/saveUser', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ name, age, school })
    })
    .then(response => response.text())
    .then(data => {
        document.getElementById('message').innerText = data;
    })
    .catch(error => {
        console.error('Error:', error);
    });
}