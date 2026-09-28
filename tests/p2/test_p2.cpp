// tests/p2/test_p2.cpp
//
// YOUR test suite goes here. At least 12 assert-based test cases — see
// spec §5 for the required categories and the sample test for the
// expected level of rigor.
//
// This file is a stub so the project builds out of the box; replace the
// body of main() with your own tests.

#include "core/conversation.h"
#include "core/message.h"
#include "core/sentinel_scanner.h"
#include "harness/harness.h"
#include "model/replay_client.h"
#include "model/scripted_client.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    // ---------------------------------------------------------
    // TEST 1: Message Initialization
    // ---------------------------------------------------------
    Message m1(Role::User, "Hello AI");
    assert(m1.role() == Role::User);
    assert(m1.content() == "Hello AI");

    // ---------------------------------------------------------
    // TEST 2: Conversation Append and Size (Dynamic Array)
    // ---------------------------------------------------------
    Conversation conv;
    assert(conv.size() == 0);
    conv.append(m1);
    assert(conv.size() == 1);
    assert(conv.at(0).content() == "Hello AI");

    // ---------------------------------------------------------
    // TEST 3: Conversation Copy Constructor (Rule of Five)
    // ---------------------------------------------------------
    Conversation conv_copy = conv; 
    assert(conv_copy.size() == 1);
    assert(conv_copy.at(0).content() == "Hello AI");
    
    // Modify the original to ensure it was a deep copy, not shallow
    conv.append(Message(Role::Assistant, "Hi there"));
    assert(conv.size() == 2);
    assert(conv_copy.size() == 1); // Copy should remain unaffected

    // ---------------------------------------------------------
    // TEST 4: SentinelScanner Feed and Flush
    // ---------------------------------------------------------
    SentinelScanner scanner("<|end|>");
    
    // Feed a chunk without the sentinel
    auto out1 = scanner.feed("Part one. ");
    assert(out1.sentinel_found == false);
    
    // Feed the rest including the sentinel
    auto out2 = scanner.feed("Part two.<|end|>");
    assert(out2.sentinel_found == true);
    
    // Check the combined stream output instead of isolated chunks
    assert(out1.safe_text + out2.safe_text == "Part one. Part two.");

    // ---------------------------------------------------------
    // TEST 5: Empty Conversation Bounds
    // ---------------------------------------------------------
    Conversation empty_conv;
    bool threw_exception = false;
    try {
        empty_conv.at(0);
    } catch (const std::out_of_range&) {
        threw_exception = true;
    }
    assert(threw_exception && "Empty conversation should throw out_of_range");

    // ---------------------------------------------------------
    // TEST 6: Rule of Five (Copy Assignment)
    // ---------------------------------------------------------
    Conversation conv_assign;
    conv_assign = conv; // Uses conv from TEST 2 (size 2)
    assert(conv_assign.size() == 2);
    assert(&conv_assign.at(0) != &conv.at(0)); // Deep copy check
    
    // Self-assignment safety
    conv_assign = conv_assign;
    assert(conv_assign.size() == 2);

    // ---------------------------------------------------------
    // TEST 7: Rule of Five (Move Semantics)
    // ---------------------------------------------------------
    Conversation conv_move_src;
    conv_move_src.append(Message(Role::System, "Move me"));
    const Message* orig_ptr = &conv_move_src.at(0);
    
    Conversation conv_move_dst(std::move(conv_move_src));
    assert(conv_move_dst.size() == 1);
    assert(&conv_move_dst.at(0) == orig_ptr); // Pointer successfully stolen
    assert(conv_move_src.size() == 0); // Source zeroed out

    // ---------------------------------------------------------
    // TEST 8: Growth Behavior (Amortized Array)
    // ---------------------------------------------------------
    Conversation conv_grow;
    for (int i = 0; i < 50; ++i) {
        conv_grow.append(Message(Role::User, "Msg " + std::to_string(i)));
    }
    assert(conv_grow.size() == 50);
    assert(conv_grow.at(0).content() == "Msg 0");
    assert(conv_grow.at(49).content() == "Msg 49");

    // ---------------------------------------------------------
    // TEST 9: Scanner (Split Sentinel Boundaries)
    // ---------------------------------------------------------
    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "Goodbye." + sentinel;
    for (std::size_t split = 0; split <= text.size(); ++split) {
        SentinelScanner split_scanner(sentinel);
        auto s_out1 = split_scanner.feed(text.substr(0, split));
        auto s_out2 = split_scanner.feed(text.substr(split));
        assert((s_out1.sentinel_found || s_out2.sentinel_found));
        assert(s_out1.safe_text + s_out2.safe_text == "Goodbye.");
    }

    // ---------------------------------------------------------
    // TEST 10: Scanner (False Alarms)
    // ---------------------------------------------------------
    SentinelScanner false_scanner("<|end_conversation|>");
    auto f_out = false_scanner.feed("This is a <|end_world|>");
    assert(!f_out.sentinel_found);

    // ---------------------------------------------------------
    // TEST 11: Scanner (Bounded Memory)
    // ---------------------------------------------------------
    SentinelScanner bound_scanner("STOP");
    std::string big_stream(1000, 'A');
    auto b_out = bound_scanner.feed(big_stream);
    assert(!b_out.sentinel_found);
    // 1000 characters fed. "STOP" is 4 chars. Max held back is 3. 
    // It must have successfully emitted at least 997 chars as safe.
    assert(b_out.safe_text.size() >= 1000 - 3);

    // ---------------------------------------------------------
    // TEST 12: Scanner (Flush Behavior)
    // ---------------------------------------------------------
    SentinelScanner flush_scanner("END");
    auto fl_out1 = flush_scanner.feed("Some text EN");
    assert(!fl_out1.sentinel_found);
    auto fl_out2 = flush_scanner.flush(); // Should emit the held "EN"
    assert(!fl_out2.sentinel_found);
    assert(fl_out1.safe_text + fl_out2.safe_text == "Some text EN");

    std::cout << "SUCCESS: All 12 test cases passed!\n";
    return 0;
}
