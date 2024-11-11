function submitData() {
    const name = document.getElementById('name').value.trim();
    const age = document.getElementById('age').value.trim();
    const school = document.getElementById('school').value.trim();
    const messageElement = document.getElementById('message');

    if (!name || !age || !school) {
        messageElement.style.color = 'red';
        messageElement.innerText = 'Please fill in all fields.';
        return;
    }
    
    if (isNaN(age) || age <= 0) {
        messageElement.style.color = 'red';
        messageElement.innerText = 'Please enter a valid age greater than 0.';
        return;
    }

    fetch('/api/saveUser', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ name, age, school })
    })
    .then(response => response.text())
    .then(data => {
        messageElement.style.color = 'green';
        messageElement.innerText = 'Data saved successfully!';
        
        document.getElementById('name').value = '';
        document.getElementById('age').value = '';
        document.getElementById('school').value = '';

        fetchUsers();
    })
    .catch(error => {
        console.error('Error:', error);
        messageElement.style.color = 'red';
        messageElement.innerText = 'Error saving data.';
    });
}

function fetchUsers() {
    fetch('/api/users')
        .then(response => response.json())
        .then(data => {
            const tableBody = document.getElementById('userTable').getElementsByTagName('tbody')[0];
            tableBody.innerHTML = '';

            data.forEach(user => {
                const row = document.createElement('tr');
                row.innerHTML = `<td>${user.name}</td><td>${user.age}</td><td>${user.school}</td>`;
                tableBody.appendChild(row);
            });
        })
        .catch(error => {
            console.error('Error fetching users:', error);
        });
}

window.onload = fetchUsers;