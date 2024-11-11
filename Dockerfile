FROM node:18

WORKDIR /app

COPY package* ./
RUN npm install

COPY . .

EXPOSE 3400

CMD ["node", "index.js"]