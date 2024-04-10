const path = require('path');

const PROJECT_ROOT = path.join(__dirname, '../..');

module.exports = (dir = '') => path.join(PROJECT_ROOT, dir);
