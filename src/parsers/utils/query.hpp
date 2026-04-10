#pragma once

#include "fmt/base.h"
#include <bitset>
#include <string_view>

enum class QueryOp : int { INVALID = -1, EQ, NEQ, GT, LT, GTE, LTE };
enum class ParseState : int { KEY = 0, VALUE };

static constexpr std::bitset<256> make_op_start_table() {
    std::bitset<256> table{};

    table.set('>');
    table.set('<');
    table.set('=');
    table.set('!');

    return table;
}

static auto OP_START_TABLE = make_op_start_table();

struct QueryToken {
    std::string_view key;
    std::string_view value;
    QueryOp op;
};

struct QueryState {
    std::string query;

    size_t key_start;
    size_t op_start;
    size_t op_end;
    size_t value_end;

    bool in_quotes = false;

    ParseState value = ParseState::KEY;
    QueryOp op = QueryOp::INVALID;
    QueryToken token = {};

    void reset() {
        value = ParseState::KEY;
        in_quotes = false;

        key_start = -1;
        op_start = 0;
        op_end = 0;
        value_end = 0;
    }
};

namespace query {
    inline std::pair<QueryOp, size_t> parse_operator(std::string_view sv, size_t pos) {
        char c1 = sv[pos];
        char c2 = sv[pos + 1];

        switch (c1) {
            case '>': {
                if (c2 == '=') {
                    return {QueryOp::GTE, 2};
                }

                return {QueryOp::GT, 1};
            }

            case '<': {
                if (c2 == '=') {
                    return {QueryOp::LTE, 2};
                }

                return {QueryOp::LT, 1};
            }

            case '=': {
                if (c2 == '=') { // variation
                    return {QueryOp::EQ, 2};
                }

                return {QueryOp::EQ, 1};
            }

            case '!': {
                if (c2 == '=') {
                    return {QueryOp::NEQ, 2};
                }

                return {QueryOp::INVALID, 1};
            }
        }

        return {QueryOp::INVALID, 0};
    }

    template <typename EvalFn>
    inline std::pair<bool, QueryState> parse(std::string_view data, EvalFn&& eval) {
        QueryState m_state = {};

        for (size_t i = 0; i < data.length(); i++) {
            bool is_last = data.length() - 1 == i;
            char c = data[i];

            switch (m_state.value) {
                case ParseState::KEY: {
                    if (m_state.key_start == -1) {
                        m_state.key_start = i;
                    }

                    // is a operator?
                    if (m_state.key_start < i && OP_START_TABLE[static_cast<unsigned char>(c)]) {
                        auto [op, size] = parse_operator(data, i);
                        m_state.op = op;

                        // invalidate duplicated op's
                        if (!is_last && c != '=' && data[i + 1] == c) {
                            m_state.query += data.substr(m_state.key_start, i - 1);
                        } else {
                            m_state.value = ParseState::VALUE;
                            m_state.op_start = i;
                            m_state.op_end = m_state.op_start + size;
                        }

                        continue;
                    }
                } break;
                case ParseState::VALUE: {
                    m_state.value_end = i;
                    bool is_char_quote = c == '"';

                    // skip on opening quote
                    if (!m_state.in_quotes && is_char_quote) {
                        m_state.in_quotes = true;
                        continue;
                    }

                    bool is_ending_quote = m_state.in_quotes && is_char_quote;
                    bool is_separator = !m_state.in_quotes && c == ' ';
                    bool is_unquoted_last = !m_state.in_quotes && is_last;

                    bool should_evaluate = is_ending_quote || is_separator || is_unquoted_last;

                    if (should_evaluate) {
                        size_t value_start = m_state.op_end;
                        size_t value_length = m_state.value_end - value_start;

                        if (is_ending_quote) {
                            value_start += 1;
                            value_length -= 1;
                        } else if (is_unquoted_last) {
                            value_length += 1; // include last char
                        }

                        m_state.token = {
                            .key = data.substr(m_state.key_start, m_state.op_start - m_state.key_start),
                            .value = data.substr(value_start, value_length),
                            .op = m_state.op,
                        };

                        if (eval(m_state.token)) {
                            return {true, m_state};
                        }

                        m_state.reset();
                    }
                } break;
            }
        }

        return {false, m_state};
    }
}