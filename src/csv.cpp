#include "csv.h"

/*
 * Simple CSV parsing
 * Credits: https://stackoverflow.com/a/1120224/1458605
 */

std::vector<std::string>
getNextLineAndSplitIntoTokens(std::istream& str)
{
    std::vector<std::string> result;
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    while (std::getline(lineStream, cell, ',')) {
        result.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty()) {
        // If there was a trailing comma then add an empty element.
        result.push_back("");
    }
    return result;
}

std::istream&
operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

const std::string& CSVRow::operator[](std::size_t index) const
{
    return m_data[index];
}

std::size_t
CSVRow::size() const
{
    return m_data.size();
}

void
CSVRow::readNextRow(std::istream& str)
{
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    m_data.clear();
    while (std::getline(lineStream, cell, ',')) {
        m_data.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty()) {
        // If there was a trailing comma then add an empty element.
        m_data.push_back("");
    }
}

CSVIterator::CSVIterator(std::istream& str)
    : m_str(str.good() ? &str : NULL)
{
    ++(*this);
}

CSVIterator::CSVIterator()
    : m_str(NULL)
{}

CSVIterator&
CSVIterator::operator++()
{
    if (m_str) {
        if (!((*m_str) >> m_row)) {
            m_str = NULL;
        }
    }
    return *this;
}

CSVIterator
CSVIterator::operator++(int)
{
    CSVIterator tmp(*this);
    ++(*this);
    return tmp;
}

const CSVRow& CSVIterator::operator*() const
{
    return m_row;
}

const CSVRow* CSVIterator::operator->() const
{
    return &m_row;
}

bool
CSVIterator::operator==(const CSVIterator& rhs)
{
    return ((this == &rhs) || ((this->m_str == NULL) && (rhs.m_str == NULL)));
}

bool
CSVIterator::operator!=(const CSVIterator& rhs)
{
    return !((*this) == rhs);
}
