//
// Created by Paul Walker on 1/7/22.
//

#include "FXPage.h"
#include "PageContentBase.h"
#include "widgets/ComboBox.h"
#include "widgets/ParamEditor.h"
#include "widgets/OutlinedTextButton.h"
#include "sst/cpputils.h"

namespace scxt
{
namespace pages
{
namespace fx_contents
{
struct SingleFX : scxt::pages::contents::PageContentBase<scxt::pages::FXPage>
{
    SingleFX(FXPage &p, int id)
        : idx(id), contents::PageContentBase<FXPage>(p, "Effect " + std::to_string(id + 1),
                                                     juce::Colour(0xFF447744))
    {
        auto &mult = parentPage.editor->multi;
        auto &fx = parentPage.editor->multi.filters[idx];
        typeSelector =
            bind<widgets::IntParamComboBox>(fx.type, parentPage.editor->multiFilterTypeNames);
        outputTarget = bind<widgets::IntParamComboBox>(mult.filter_output[idx],
                                                       parentPage.editor->multiFilterOutputNames);
        muteButton = bind<widgets::IntParamToggleButton>(fx.bypass, "mute");

        for (auto i = 0; i < n_filter_parameters; ++i)
            fParams[i] = bindFloatHSlider(fx.p[i]);
        for (auto i = 0; i < n_filter_iparameters; ++i)
            iParams[i] =
                bind<widgets::IntParamMultiSwitch>(widgets::IntParamMultiSwitch::VERT, fx.ip[i]);
    }

    ~SingleFX() = default;

    void resized() override
    {
        auto b = getContentsBounds();
        auto bl = b.withTrimmedRight(50).reduced(1, 1);
        auto br = b.withTrimmedLeft(b.getWidth() - 48).reduced(1, 1);

        auto rgl = contents::RowGenerator(bl, 4 + n_filter_parameters);
        auto rgr = contents::RowGenerator(br, 4 + n_filter_parameters);

        typeSelector->setBounds(rgl.next());
        rgl.next();
        for (const auto &q : fParams)
            q->setBounds(rgl.next());
        rgl.next();
        outputTarget->setBounds(rgl.next());

        muteButton->setBounds(rgr.next());
        rgr.next();
        for (const auto &p : iParams)
            p->setBounds(rgr.next(5));
    }

    int idx{-1};

    std::array<std::unique_ptr<widgets::FloatParamSlider>, n_filter_parameters> fParams;
    std::array<std::unique_ptr<widgets::IntParamMultiSwitch>, n_filter_iparameters> iParams;

    std::unique_ptr<widgets::IntParamComboBox> typeSelector;
    std::unique_ptr<widgets::IntParamToggleButton> muteButton;

    std::unique_ptr<widgets::IntParamComboBox> outputTarget;
};
} // namespace fx_contents
FXPage::FXPage(SCXTEditor *e, SCXTEditor::Pages p) : PageBase(e, p)
{
    for (auto i = 0; i < num_fxunits; ++i)
    {
        auto q = makeContent<fx_contents::SingleFX>(*this, i);
        fxComponents[i] = std::move(q);
    }
}
FXPage::~FXPage() = default;

void FXPage::resized()
{
    jassert(num_fxunits == 8);
    auto w = getWidth() / 4;
    auto h = getHeight() / 2;

    auto r = getLocalBounds().withWidth(w).withHeight(h);
    for (int i = 0; i < num_fxunits; ++i)
    {
        fxComponents[i]->setBounds(r.reduced(1, 1));
        r = r.translated(w, 0);
        if (i == 3)
            r = r.translated(-4 * w, h);
    }
}

} // namespace pages
} // namespace scxt